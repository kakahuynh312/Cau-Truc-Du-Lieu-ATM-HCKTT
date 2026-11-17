#include "ATM.h"

#include "ConsoleUtil.h"
#include "MenuAdmin.h"
#include "MenuUser.h"

using namespace std;

namespace {
	const string DEFAULT_PIN = "123456";

	// So lan nhap sai toi da truoc khi khoa the va thoat he thong
	const int MAX_LOGIN_ATTEMPTS = 3;

	// Do dai ma PIN hop le
	const size_t PIN_LENGTH = 6;

	// Menu chinh
	const int MAIN_ADMIN = 1;
	const int MAIN_USER = 2;
	const int MAIN_THOAT = 3;

	int docLuaChon(int iMin, int iMax) {
		while (true) {
			const long long llLuaChon = ConsoleUtil::nhapSoNguyen(" Lua chon: ");
			if (llLuaChon >= iMin && llLuaChon <= iMax) {
				return static_cast<int>(llLuaChon);
			}
			ConsoleUtil::inLoi("Lua chon khong hop le, vui long nhap lai!");
		}
	}
}

ATM::ATM() {
	this->_bThoat = false;
}

void ATM::chay() {
	if (!this->taiDuLieu()) {
		ConsoleUtil::inLoi("Khong the doc du lieu he thong. Chuong trinh dung.");
		ConsoleUtil::tamDung();
		return;
	}

	while (!this->_bThoat) {
		ConsoleUtil::xoaManHinh();
		this->hienThiMenuChinh();

		switch (docLuaChon(MAIN_ADMIN, MAIN_THOAT)) {
		case MAIN_ADMIN:
			this->dangNhapAdmin();
			break;
		case MAIN_USER:
			this->dangNhapUser();
			break;
		case MAIN_THOAT:
			this->_bThoat = true;
			break;
		default:
			break;
		}
	}
}

bool ATM::taiDuLieu() {
	// QuanLyAdmin tu tao Admin.txt (3 admin) neu chua co
	if (!this->_quanLyAdmin.taiDuLieu()) {
		return false;
	}

	// QuanLyTheTu tu tao TheTu.txt (10 the tu) cung file [ID].txt, LichSu[ID].txt neu chua co
	return this->_quanLyTheTu.taiDuLieu();
}

void ATM::hienThiMenuChinh() const {
	ConsoleUtil::inTieuDe("MAY ATM");
	ConsoleUtil::inLuaChon(" 1. Dang nhap Admin");
	ConsoleUtil::inLuaChon(" 2. Dang nhap User");
	ConsoleUtil::inLuaChon(" 3. Thoat");
}

void ATM::dangNhapAdmin() {
	ConsoleUtil::xoaManHinh();

	// QuanLyAdmin tu hien man hinh DANG NHAP ADMIN, nhap User/Pin (an *)
	if (!this->_quanLyAdmin.dangNhapAdmin()) {
		return;
	}

	MenuAdmin menuAdmin(this->_quanLyTheTu);
	menuAdmin.chay();
}

void ATM::dangNhapUser() {
	ConsoleUtil::xoaManHinh();
	ConsoleUtil::inTieuDe("DANG NHAP USER");

	for (int iLanThu = 0; iLanThu < MAX_LOGIN_ATTEMPTS; ++iLanThu) {
		const string strID = ConsoleUtil::nhapDong(" ID: ");
		const string strPin = ConsoleUtil::nhapMatKhau(" Pin: ", PIN_LENGTH, true);

		TheTu* pTheTu = this->_quanLyTheTu.timTheoID(strID);
		if (pTheTu != nullptr && pTheTu->getBiKhoa()) {
			// The da bi khoa tu truoc (trang thai doc tu TheTu.txt): ve menu chinh
			ConsoleUtil::inLoi("The da bi khoa. Vui long lien he Admin de mo khoa.");
			ConsoleUtil::tamDung();
			return;
		}

		// kiemTraDangNhap dem sai rieng tung ID; du 3 lan thi khoa the va ghi "Da khoa" vao TheTu.txt
		if (this->_quanLyTheTu.kiemTraDangNhap(strID, strPin)) {
			if (pTheTu->getDoiPINLanDau() && !this->doiPinLanDau(*pTheTu)) {
				return;
			}

			MenuUser menuUser(this->_quanLyTheTu, strID);
			menuUser.chay();
			return;
		}

		// The vua bi khoa o lan sai thu 3 (thong bao da duoc in): thoat vong lap
		if (pTheTu != nullptr && pTheTu->getBiKhoa()) {
			break;
		}
	}

	// Nhap sai 3 lan: thoat khoi he thong (the o tren, neu co, da bi khoa)
	ConsoleUtil::inLoi("Nhap sai qua " + to_string(MAX_LOGIN_ATTEMPTS) + " lan. The da bi khoa.");
	ConsoleUtil::tamDung();
}

bool ATM::doiPinLanDau(TheTu& theTu) {
	ConsoleUtil::xoaManHinh();
	ConsoleUtil::inTieuDe("DOI MA PIN LAN DAU");
	ConsoleUtil::inCanhBao(" Day la lan dang nhap dau tien, ban phai doi ma PIN.");

	for (int iLanThu = 0; iLanThu < MAX_LOGIN_ATTEMPTS; ++iLanThu) {
		if (this->nhapVaDoiPin(theTu)) {
			ConsoleUtil::tamDung();
			return true;
		}
	}

	ConsoleUtil::inLoi("Chua doi duoc ma PIN. Phien dang nhap bi huy.");
	ConsoleUtil::tamDung();
	return false;
}

bool ATM::nhapVaDoiPin(TheTu& theTu) {
	const string strPinMoi = ConsoleUtil::nhapMatKhau(" Ma PIN moi: ", PIN_LENGTH, true);
	const string strXacNhan = ConsoleUtil::nhapMatKhau(" Nhap lai ma PIN moi: ", PIN_LENGTH, true);

	const string strLoi = this->kiemTraPinMoi(theTu, strPinMoi, strXacNhan);
	if (!strLoi.empty()) {
		ConsoleUtil::inLoi(strLoi);
		return false;
	}

	// doiPIN cap nhat the tu va ghi lai TheTu.txt (hoan tac neu ghi file loi)
	if (!this->_quanLyTheTu.doiPIN(theTu.getID(), theTu.getPIN(), strPinMoi, strXacNhan)) {
		ConsoleUtil::inLoi("Khong the doi ma PIN. Giao dich bi huy!");
		return false;
	}

	ConsoleUtil::inThanhCong("Doi ma PIN thanh cong!");
	return true;
}

string ATM::kiemTraPinMoi(const TheTu& theTu,
	const string& strPinMoi,
	const string& strXacNhan) const {
	if (!TheTu::kiemTraDinhDangPIN(strPinMoi)) {
		return "Ma PIN phai gom dung " + to_string(PIN_LENGTH) + " chu so!";
	}
	if (strPinMoi == theTu.getPIN()) {
		return "Ma PIN moi khong duoc giong ma PIN cu!";
	}
	if (strPinMoi == DEFAULT_PIN) {
		return "Ma PIN moi khong duoc giong ma PIN mac dinh!";
	}
	if (strPinMoi != strXacNhan) {
		return "Hai lan nhap ma PIN moi khong khop!";
	}
	return "";
}
