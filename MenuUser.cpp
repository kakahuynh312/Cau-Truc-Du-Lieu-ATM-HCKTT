#include "MenuUser.h"
#include <iomanip>
#include "ConsoleUtil.h"
#include "TheTu.h"
#include "QuanLyTaiKhoan.h"
#include "TaiKhoan.h"
#include "GiaoDich.h"

using namespace std;

namespace {
    const int USER_XEM_THONG_TIN = 1;
    const int USER_RUT_TIEN = 2;
    const int USER_CHUYEN_TIEN = 3;
    const int USER_XEM_GIAO_DICH = 4;
    const int USER_DOI_MA_PIN = 5;
    const int USER_THOAT = 6;

    int docLuaChon(int iMin, int iMax) {
        while (true) {
            const long long llLuaChon =
                ConsoleUtil::nhapSoNguyen(" Lua chon: ");

            if (llLuaChon >= iMin && llLuaChon <= iMax) {
                return static_cast<int>(llLuaChon);
            }

            ConsoleUtil::inLoi(
                "Lua chon khong hop le, vui long nhap lai!"
            );
        }
    }
}

// Constructor
MenuUser::MenuUser(
    QuanLyTheTu& quanLyTheTu,
    const string& strID
) : _quanLyTheTu(quanLyTheTu), _strID(strID) {
}

// Chay menu
void MenuUser::chay() {
    bool bTiepTuc = true;

    while (bTiepTuc) {
        ConsoleUtil::xoaManHinh();
        this->hienThiMenu();

        switch (docLuaChon(USER_XEM_THONG_TIN, USER_THOAT)) {
        case USER_XEM_THONG_TIN:
            this->xemThongTin();
            break;

        case USER_RUT_TIEN:
            this->rutTien();
            break;

        case USER_CHUYEN_TIEN:
            this->chuyenTien();
            
            break;

        case USER_XEM_GIAO_DICH:
            this->xemGiaoDich();
            break;

        case USER_DOI_MA_PIN:
            this->doiMaPin();
            break;

        case USER_THOAT:
            bTiepTuc = false;
            break;

        default:
            break;
        }
    }
}

// Hien thi menu
void MenuUser::hienThiMenu() const {
    ConsoleUtil::inTieuDe("MENU USER");

    ConsoleUtil::inLuaChon(" 1. Xem thong tin tai khoan");
    ConsoleUtil::inLuaChon(" 2. Rut tien");
    ConsoleUtil::inLuaChon(" 3. Chuyen tien");
    ConsoleUtil::inLuaChon(" 4. Xem lich su giao dich");
    ConsoleUtil::inLuaChon(" 5. Doi ma PIN");
    ConsoleUtil::inLuaChon(" 6. Thoat");
}

void MenuUser::xemThongTin() {
    ConsoleUtil::xoaManHinh();
    ConsoleUtil::inTieuDe("THONG TIN TAI KHOAN");

    QuanLyTaiKhoan quanLyTaiKhoan;
    TaiKhoan tk;

    // Doc lai so du moi nhat tu file
    if (!quanLyTaiKhoan.docFile(_strID, tk)) {
        ConsoleUtil::inLoi("Khong tim thay tai khoan!");
    }
    else {
        quanLyTaiKhoan.xuatThongTin(tk);
    }

    ConsoleUtil::tamDung();
}

void MenuUser::doiMaPin() {
    ConsoleUtil::xoaManHinh();
    ConsoleUtil::inTieuDe("DOI MA PIN");

    string strPINCu = ConsoleUtil::nhapDong(" Nhap PIN cu: ");
    string strPINMoi1 = ConsoleUtil::nhapDong(" Nhap PIN moi: ");
    string strPINMoi2 = ConsoleUtil::nhapDong(" Nhap lai PIN moi: ");

    if (_quanLyTheTu.doiPIN(
        _strID,
        strPINCu,
        strPINMoi1,
        strPINMoi2
    )) {
        ConsoleUtil::inThanhCong("Doi ma PIN thanh cong!");
    }
    else {
        ConsoleUtil::inLoi("Doi ma PIN that bai!");
    }

    ConsoleUtil::tamDung();
}
bool MenuUser::kiemTraSoTien(long long llSoTien) const {
    if (llSoTien <= 0) {
        ConsoleUtil::inLoi("So tien phai lon hon 0!");
        return false;
    }

    return true;
}

void MenuUser::rutTien() {
    ConsoleUtil::xoaManHinh();
    ConsoleUtil::inTieuDe("RUT TIEN");

    QuanLyTaiKhoan quanLyTaiKhoan;
    TaiKhoan tk;

    if (!quanLyTaiKhoan.docFile(_strID, tk)) {
        ConsoleUtil::inLoi("Khong tim thay tai khoan!");
        ConsoleUtil::tamDung();
        return;
    }

    cout << fixed << setprecision(0);
    cout << " So du hien tai: " << tk.getSoDu()
         << " " << tk.getLoaiTienTe() << '\n';

    long long llSoTien = ConsoleUtil::nhapSoNguyen(
        " Nhap so tien can rut: "
    );

    if (!kiemTraSoTien(llSoTien)) {
        ConsoleUtil::inLoi("So tien rut khong hop le!");
        ConsoleUtil::tamDung();
        return;
    }

    if (llSoTien > tk.getSoDu()) {
        ConsoleUtil::inLoi("So du khong du de thuc hien giao dich!");
        ConsoleUtil::tamDung();
        return;
    }

    double soDuCu = tk.getSoDu();

    // Cap nhat so du
    tk.setSoDu(soDuCu - llSoTien);

    if (!quanLyTaiKhoan.ghiFile(tk)) {
        ConsoleUtil::inLoi("Khong the cap nhat so du!");
        ConsoleUtil::tamDung();
        return;
    }

    // Tao giao dich rut tien
    GiaoDich giaoDich(
        _strID,
        1,
        llSoTien,
        "",
        "",
        GiaoDich::layThoiGianHienTai()
    );

    // Luu lich su giao dich
    if (!_quanLyGiaoDich.ghiThemChoId(_strID, giaoDich)) {
        tk.setSoDu(soDuCu);
        quanLyTaiKhoan.ghiFile(tk);

        ConsoleUtil::inLoi("Khong the luu lich su giao dich!");
        ConsoleUtil::tamDung();
        return;
    }

    ConsoleUtil::inThanhCong("Rut tien thanh cong!");

    cout << " So tien da rut: " << llSoTien
         << " " << tk.getLoaiTienTe() << '\n';

    cout << " So du con lai: " << tk.getSoDu()
         << " " << tk.getLoaiTienTe() << '\n';

    ConsoleUtil::tamDung();
}

void MenuUser::chuyenTien() {
    ConsoleUtil::xoaManHinh();
    ConsoleUtil::inTieuDe("CHUYEN TIEN");

    QuanLyTaiKhoan quanLyTaiKhoan;

    TaiKhoan tkGui;
    TaiKhoan tkNhan;

    // Doc tai khoan nguoi gui
    if (!quanLyTaiKhoan.docFile(_strID, tkGui)) {
        ConsoleUtil::inLoi("Khong tim thay tai khoan nguoi gui!");
        ConsoleUtil::tamDung();
        return;
    }

    cout << " So du hien tai: " << fixed << setprecision(0)
         << tkGui.getSoDu() << " " << tkGui.getLoaiTienTe() << '\n';

    // Nhap ID nguoi nhan
    string strIDNhan;

    cout << " Nhap ID nguoi nhan: ";
    cin >> strIDNhan;

    if (strIDNhan == _strID) {
        ConsoleUtil::inLoi("Khong the chuyen tien cho chinh minh!");
        ConsoleUtil::tamDung();
        return;
    }

    // Doc tai khoan nguoi nhan
    if (!quanLyTaiKhoan.docFile(strIDNhan, tkNhan)) {
        ConsoleUtil::inLoi("Tai khoan nguoi nhan khong ton tai!");
        ConsoleUtil::tamDung();
        return;
    }

    // Kiem tra cung loai tien te
    if (tkGui.getLoaiTienTe() != tkNhan.getLoaiTienTe()) {
        ConsoleUtil::inLoi("Hai tai khoan khong cung loai tien te!");
        ConsoleUtil::tamDung();
        return;
    }

    // Nhap so tien chuyen
    long long llSoTien = ConsoleUtil::nhapSoNguyen(
        " Nhap so tien can chuyen: "
    );

    if (!kiemTraSoTien(llSoTien)) {
        ConsoleUtil::inLoi("So tien chuyen khong hop le!");
        ConsoleUtil::tamDung();
        return;
    }

    if (llSoTien > tkGui.getSoDu()) {
        ConsoleUtil::inLoi("So du khong du de chuyen tien!");
        ConsoleUtil::tamDung();
        return;
    }

    double soDuGuiCu = tkGui.getSoDu();
    double soDuNhanCu = tkNhan.getSoDu();

    // Tinh so du moi
    tkGui.setSoDu(soDuGuiCu - llSoTien);
    tkNhan.setSoDu(soDuNhanCu + llSoTien);

    // Cap nhat file nguoi gui
    if (!quanLyTaiKhoan.ghiFile(tkGui)) {
        ConsoleUtil::inLoi("Khong the cap nhat tai khoan nguoi gui!");
        ConsoleUtil::tamDung();
        return;
    }

    // Cap nhat file nguoi nhan
    if (!quanLyTaiKhoan.ghiFile(tkNhan)) {
        tkGui.setSoDu(soDuGuiCu);
        quanLyTaiKhoan.ghiFile(tkGui);

        ConsoleUtil::inLoi("Khong the cap nhat tai khoan nguoi nhan!");
        ConsoleUtil::tamDung();
        return;
    }

    string strThoiGian = GiaoDich::layThoiGianHienTai();

    // Giao dich cua nguoi gui
    GiaoDich gdGui(
        _strID,
        2,
        llSoTien,
        strIDNhan,
        tkNhan.getTenChuTaiKhoan(),
        strThoiGian
    );

    // Giao dich cua nguoi nhan
    GiaoDich gdNhan(
        strIDNhan,
        3,
        llSoTien,
        _strID,
        tkGui.getTenChuTaiKhoan(),
        strThoiGian
    );

    // Luu lich su giao dich
    bool luuGui = _quanLyGiaoDich.ghiThemChoId(_strID, gdGui);

    QuanLyGiaoDich lichSuNguoiNhan;
    bool luuNhan = lichSuNguoiNhan.ghiThemChoId(strIDNhan, gdNhan);

    if (!luuGui || !luuNhan) {
        ConsoleUtil::inLoi(
            "Chuyen tien da cap nhat so du nhung luu lich su that bai. Can kiem tra lai du lieu!"
        );
        ConsoleUtil::tamDung();
        return;
    }

    ConsoleUtil::inThanhCong("Chuyen tien thanh cong!");

    cout << fixed << setprecision(0);

    cout << " Nguoi nhan: " << tkNhan.getTenChuTaiKhoan() << '\n';
    cout << " ID nguoi nhan: " << strIDNhan << '\n';
    cout << " So tien da chuyen: " << llSoTien
         << " " << tkGui.getLoaiTienTe() << '\n';

    cout << " So du cua ban: " << tkGui.getSoDu()
         << " " << tkGui.getLoaiTienTe() << '\n';

    ConsoleUtil::tamDung();
}

void MenuUser::xemGiaoDich() {
    ConsoleUtil::xoaManHinh();
    ConsoleUtil::inTieuDe("LICH SU GIAO DICH");

    if (!_quanLyGiaoDich.docFile(_strID)) {
        ConsoleUtil::inLoi("Khong doc duoc lich su giao dich!");
    }
    else if (_quanLyGiaoDich.kichThuoc() == 0) {
        ConsoleUtil::inThongBao("Tai khoan chua co giao dich nao!");
    }
    else {
        _quanLyGiaoDich.hienThiTatCa();
    }

    ConsoleUtil::tamDung();
}