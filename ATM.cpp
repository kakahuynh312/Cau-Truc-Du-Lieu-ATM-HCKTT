#include "ATM.h"

#include <filesystem>
#include <system_error>

#include "ConsoleUtil.h"
#include "MenuAdmin.h"
#include "MenuUser.h"
#include "QuanLyTaiKhoan.h"

using namespace std;

namespace {
	// Duong dan tuong doi toi thu muc va file du lieu
	const string DIR_DATA = "data";
	const string FILE_THE_TU = "data/TheTu.txt";

	const string DEFAULT_PIN = "123456";
	const string DEFAULT_CURRENCY = "VND";

	// So lan nhap sai toi da truoc khi thoat / huy
	const int MAX_LOGIN_ATTEMPTS = 3;

	// Do dai ma PIN hop le
	const size_t PIN_LENGTH = 6;

	// Menu chinh
	const int MAIN_ADMIN = 1;
	const int MAIN_USER = 2;
	const int MAIN_THOAT = 3;



	/**********************************************************
	* @Description Nhap lua chon menu trong khoang [iMin, iMax],
	*              lap lai den khi hop le
	**********************************************************/
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
    error_code errCode;
    filesystem::create_directories(DIR_DATA, errCode);

    if (!this->_quanLyAdmin.taiDuLieu()) {
        return false;
    }

    bool bDocDuoc = this->_quanLyTheTu.docFile(FILE_THE_TU);

    if (bDocDuoc && this->_quanLyTheTu.getSoLuongThe() > 0) {
        return true;
    }

    return this->khoiTaoDuLieuMau();
}

bool ATM::khoiTaoDuLieuMau() {
	int iSoTheTao = 0;
	for (int iIndex = 0; iIndex < SEED_COUNT; ++iIndex) {
		if (this->_quanLyTheTu.themTaiKhoan(SEED_ID[iIndex], SEED_NAME[iIndex], SEED_BALANCE[iIndex], DEFAULT_CURRENCY)) {
			++iSoTheTao;
		}
	}
	return iSoTheTao == SEED_COUNT;
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
			ConsoleUtil::inLoi("The da bi khoa. Vui long lien he Admin de mo khoa.");
			ConsoleUtil::tamDung();
			return;
		}

		// kiemTraDangNhap tu dem so lan sai, khoa the va in thong bao
		if (this->_quanLyTheTu.kiemTraDangNhap(strID, strPin)) {

			if (pTheTu->getDoiPINLanDau() && !this->doiPinLanDau(*pTheTu)) {
				return;
			}

			MenuUser menuUser(this->_quanLyTheTu, strID);
			menuUser.chay();

			return;
		}

		// The vua bi khoa o lan nhap sai thu 3 cua chinh the nay
		if (pTheTu != nullptr && pTheTu->getBiKhoa()) {
			// TODO: Need to review and check again (trang thai khoa chua duoc luu vao TheTu.txt)
			break;
		}
	}

	ConsoleUtil::inLoi("Nhap sai qua " + to_string(MAX_LOGIN_ATTEMPTS) + " lan. He thong se thoat.");
	ConsoleUtil::tamDung();
	this->_bThoat = true;
}


bool ATM::doiPinLanDau(TheTu& theTu) {
	ConsoleUtil::xoaManHinh();
	ConsoleUtil::inTieuDe("DOI MA PIN LAN DAU");
	ConsoleUtil::inLoi(" Day la lan dang nhap dau tien, ban phai doi ma PIN.");

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

void ATM::doiPinTuMenu(TheTu& theTu) {
	ConsoleUtil::xoaManHinh();
	ConsoleUtil::inTieuDe("DOI MA PIN");

	const string strPinCu = ConsoleUtil::nhapMatKhau(" Nhap ma PIN cu: ", PIN_LENGTH, true);
	if (!theTu.kiemTraPIN(strPinCu)) {
		ConsoleUtil::inLoi("Ma PIN cu khong chinh xac! Giao dich bi huy.");
		ConsoleUtil::tamDung();
		return;
	}

	this->nhapVaDoiPin(theTu);
	ConsoleUtil::tamDung();
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
