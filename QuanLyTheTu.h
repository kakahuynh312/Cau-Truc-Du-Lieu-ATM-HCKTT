#ifndef QUANLYTHETU_H
#define QUANLYTHETU_H

#include <string>
#include <map>

#include "TheTu.h"
#include "LinkedList.h"

using namespace std;

class QuanLyTheTu{
private:
    LinkedList<TheTu> _lstDanhSachThe;
    map<string, int> _mapSoLanNhapSai;
public:
    bool docFile(const string& strDuongDan = "data/TheTu.txt");
    bool ghiFile(const string& strDuongDan = "data/TheTu.txt") const;
    bool khoiTao10TheMacDinh(const string& strDuongDan = "data/TheTu.txt");
    void hienThiDanhSach() const;

    TheTu* timTheoID(const string& strID);
    const TheTu* timTheoID(const string& strID) const;

    bool tonTai(const string& strID) const;
    bool themTaiKhoan(const string& strID, const string& strTenTaiKhoan, long long llSoDu = 0, const string& strLoaiTien = "VND");
    bool xoaTheTu(const string& strID);
    bool moKhoa(const string& strID);
    bool kiemTraDangNhap(const string& strID,const string& strPIN);
    bool doiPIN(const string& strID, const string& strPINCu, const string& strPINMoi1, const string& strPINMoi2);

    int getSoLuongThe() const;
};

#endif