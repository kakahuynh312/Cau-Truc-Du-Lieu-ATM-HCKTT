#ifndef MENUUSER_H
#define MENUUSER_H

#include "QuanLyTheTu.h"
#include "QuanLyGiaoDich.h"

#include <string>

using namespace std;

class MenuUser {
private:
    QuanLyTheTu& _quanLyTheTu;
    QuanLyGiaoDich _quanLyGiaoDich;
    string _strID;

    void xemThongTin();
    void rutTien();
    void chuyenTien();
    void xemGiaoDich();
    void doiMaPin();

    bool kiemTraSoTien(long long llSoTien) const;

public:
    MenuUser(QuanLyTheTu& quanLyTheTu, const string& strID);

    void chay();
    void hienThiMenu() const;
};

#endif