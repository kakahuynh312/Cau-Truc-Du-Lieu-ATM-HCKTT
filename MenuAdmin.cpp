#include "MenuAdmin.h"

#include "ConsoleUtil.h"
#include "TheTu.h"

using namespace std;

namespace {
	// Ma PIN mac dinh cua tai khoan moi (chi dung de thong bao)
	const string DEFAULT_PIN = "123456";

	// Menu Admin
	const int ADMIN_XEM_DANH_SACH = 1;
	const int ADMIN_THEM_TAI_KHOAN = 2;
	const int ADMIN_XOA_TAI_KHOAN = 3;
	const int ADMIN_MO_KHOA = 4;
	const int ADMIN_THOAT = 5;

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

// Thanh vien tham chieu (QuanLyTheTu&) bat buoc khoi tao bang danh sach khoi tao
MenuAdmin::MenuAdmin(QuanLyTheTu& quanLyTheTu)
	: _quanLyTheTu(quanLyTheTu) {
}

void MenuAdmin::chay() {
	bool bTiepTuc = true;
	while (bTiepTuc) {
		ConsoleUtil::xoaManHinh();
		this->hienThiMenu();

		switch (docLuaChon(ADMIN_XEM_DANH_SACH, ADMIN_THOAT)) {
		case ADMIN_XEM_DANH_SACH:
			this->xemDanhSach();
			break;
		case ADMIN_THEM_TAI_KHOAN:
			this->themTaiKhoan();
			break;
		case ADMIN_XOA_TAI_KHOAN:
			this->xoaTaiKhoan();
			break;
		case ADMIN_MO_KHOA:
			this->moKhoaTaiKhoan();
			break;
		case ADMIN_THOAT:
			bTiepTuc = false;
			break;
		default:
			break;
		}
	}
}

void MenuAdmin::hienThiMenu() const {
	ConsoleUtil::inTieuDe("MENU ADMIN");
	ConsoleUtil::inLuaChon(" 1. Xem danh sach tai khoan");
	ConsoleUtil::inLuaChon(" 2. Them tai khoan");
	ConsoleUtil::inLuaChon(" 3. Xoa tai khoan");
	ConsoleUtil::inLuaChon(" 4. Mo khoa tai khoan");
	ConsoleUtil::inLuaChon(" 5. Thoat");
}

void MenuAdmin::xemDanhSach() const {
	ConsoleUtil::xoaManHinh();
	ConsoleUtil::inTieuDe("DANH SACH TAI KHOAN");
	this->_quanLyTheTu.hienThiDanhSach();
	ConsoleUtil::tamDung();
}

void MenuAdmin::themTaiKhoan() {
    ConsoleUtil::xoaManHinh();
    ConsoleUtil::inTieuDe("THEM TAI KHOAN");

    string strId;
    if (!this->nhapId(strId)) {
        return;
    }

    if (this->_quanLyTheTu.tonTai(strId)) {
        ConsoleUtil::inThongBao("ID da ton tai!");
        ConsoleUtil::tamDung();
        return;
    }

    const string strTen = ConsoleUtil::nhapDong(" Ten tai khoan: ");
    if (strTen.empty()) {
        ConsoleUtil::inLoi("Ten tai khoan khong duoc de trong!");
        ConsoleUtil::tamDung();
        return;
    }

    const long long llSoDu =
        ConsoleUtil::nhapSoNguyen(" So du ban dau: ");

    string strLoaiTien;
    while (true) {
        strLoaiTien = ConsoleUtil::nhapDong(
            " Don vi tien te (VND/USD/EUR): "
        );

        if (strLoaiTien == "VND" ||
            strLoaiTien == "USD" ||
            strLoaiTien == "EUR") {
            break;
        }

        ConsoleUtil::inLoi(
            "Don vi tien te khong hop le! Vui long nhap VND, USD hoac EUR."
        );
    }

    if (this->_quanLyTheTu.themTaiKhoan(
        strId, strTen, llSoDu, strLoaiTien
    )) {
        ConsoleUtil::inThanhCong(
            "Them tai khoan thanh cong! Ma PIN mac dinh: " + DEFAULT_PIN
        );
    }
    else {
        ConsoleUtil::inLoi("Them tai khoan that bai!");
    }

    ConsoleUtil::tamDung();
}

void MenuAdmin::xoaTaiKhoan() {
	ConsoleUtil::xoaManHinh();
	ConsoleUtil::inTieuDe("XOA TAI KHOAN");

	string strId;
	if (!this->nhapId(strId)) {
		return;
	}

	if (!this->_quanLyTheTu.tonTai(strId)) {
		ConsoleUtil::inLoi("Khong tim thay tai khoan co ID nay!");
		ConsoleUtil::tamDung();
		return;
	}

	if (!ConsoleUtil::xacNhan(" Ban co chac chan muon xoa tai khoan nay")) {
		ConsoleUtil::inThongBao(" Da huy thao tac.");
		ConsoleUtil::tamDung();
		return;
	}

	if (this->_quanLyTheTu.xoaTheTu(strId)) {
		ConsoleUtil::inThanhCong("Xoa tai khoan thanh cong!");
	}
	else {
		ConsoleUtil::inLoi("Xoa tai khoan that bai!");
	}
	ConsoleUtil::tamDung();
}

void MenuAdmin::moKhoaTaiKhoan() {
	ConsoleUtil::xoaManHinh();
	ConsoleUtil::inTieuDe("MO KHOA TAI KHOAN");

	string strId;
	if (!this->nhapId(strId)) {
		return;
	}

	const TheTu* pTheTu = this->_quanLyTheTu.timTheoID(strId);
	if (pTheTu == nullptr) {
		ConsoleUtil::inLoi("Khong tim thay tai khoan co ID nay!");
		ConsoleUtil::tamDung();
		return;
	}

	if (!pTheTu->getBiKhoa()) {
		ConsoleUtil::inThongBao("Tai khoan nay khong bi khoa!");
		ConsoleUtil::tamDung();
		return;
	}

	if (this->_quanLyTheTu.moKhoa(strId)) {
		ConsoleUtil::inThanhCong("Mo khoa tai khoan thanh cong!");
	}
	else {
		ConsoleUtil::inLoi("Mo khoa tai khoan that bai!");
	}
	ConsoleUtil::tamDung();
}

bool MenuAdmin::nhapId(string& strId) const {
	strId = ConsoleUtil::nhapDong(" ID (14 chu so): ");

	if (!TheTu::kiemTraID(strId)) {
		ConsoleUtil::inLoi("ID phai gom dung 14 chu so!");
		ConsoleUtil::tamDung();
		return false;
	}
	return true;
}