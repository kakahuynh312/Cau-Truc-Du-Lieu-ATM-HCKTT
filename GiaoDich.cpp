#include "GiaoDich.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

namespace
{
	// Ky tu phan cach cac truong trong mot dong cua file lich su
	const char DELIMITER = '|';

	// Dinh dang thoi gian: dd/mm/yyyy HH:MM:SS
	const char* TIME_FORMAT = "%d/%m/%Y %H:%M:%S";
	const size_t TIME_BUFFER_SIZE = 32;

	// So chu so moi nhom khi in so tien (100.000)
	const int MONEY_GROUP_SIZE = 3;

	// Do rong cac cot khi in bang lich su
	const int WIDTH_THOI_GIAN = 21;
	const int WIDTH_LOAI = 13;
	const int WIDTH_SO_TIEN = 16;

	/**********************************************************
	* @Description Doi so nguyen thanh chuoi co dau cham phan cach hang nghin
	**********************************************************/
	string dinhDangTien(long long llSoTien)
	{
		string strSo = to_string(llSoTien);
		string strKetQua;
		int iDem = 0;

		for (int iIndex = static_cast<int>(strSo.size()) - 1; iIndex >= 0; --iIndex)
		{
			strKetQua.insert(strKetQua.begin(), strSo[iIndex]);
			++iDem;
			if (iDem == MONEY_GROUP_SIZE && iIndex > 0)
			{
				strKetQua.insert(strKetQua.begin(), '.');
				iDem = 0;
			}
		}
		return strKetQua;
	}

	/**********************************************************
	* @Description Kiem tra so nguyen co phai loai giao dich hop le khong
	*
	* @return true neu la 1 (rut), 2 (chuyen) hoac 3 (nhan)
	**********************************************************/
	bool laLoaiHopLe(int iLoai)
	{
		switch (iLoai)
		{
		case 1: // Rut tien
		case 2: // Chuyen tien
		case 3: // Nhan tien
			return true;
		default:
			return false;
		}
	}
}

GiaoDich::GiaoDich()
	: _strId(""), _iLoai(1), _llSoTien(0),
	  _strIdDoiTac(""), _strTenDoiTac(""), _strThoiGian("")
{
}

GiaoDich::GiaoDich(const string& strId,
				   int iLoai,
				   long long llSoTien,
				   const string& strIdDoiTac,
				   const string& strTenDoiTac,
				   const string& strThoiGian)
	: _strId(strId), _iLoai(iLoai), _llSoTien(llSoTien),
	  _strIdDoiTac(strIdDoiTac), _strTenDoiTac(strTenDoiTac), _strThoiGian(strThoiGian)
{
}

string GiaoDich::getId() const
{
	return this->_strId;
}

int GiaoDich::getLoai() const
{
	return this->_iLoai;
}

long long GiaoDich::getSoTien() const
{
	return this->_llSoTien;
}

string GiaoDich::getIdDoiTac() const
{
	return this->_strIdDoiTac;
}

string GiaoDich::getTenDoiTac() const
{
	return this->_strTenDoiTac;
}

string GiaoDich::getThoiGian() const
{
	return this->_strThoiGian;
}

/**********************************************************
* @Description Lay ten loai giao dich de hien thi
**********************************************************/
string GiaoDich::layTenLoai() const
{
	switch (this->_iLoai)
	{
	case 1:
		return "Rut tien";
	case 2:
		return "Chuyen tien";
	case 3:
		return "Nhan tien";
	default:
		break;
	}
	return "Khong xac dinh";
}

/**********************************************************
* @Description Chuyen giao dich thanh mot dong de ghi vao file lich su
**********************************************************/
string GiaoDich::chuyenThanhDong() const
{
	ostringstream dongGhi;
	dongGhi << this->_strId << DELIMITER
			<< this->_iLoai << DELIMITER
			<< this->_llSoTien << DELIMITER
			<< this->_strIdDoiTac << DELIMITER
			<< this->_strTenDoiTac << DELIMITER
			<< this->_strThoiGian;
	return dongGhi.str();
}

/**********************************************************
* @Description Doc giao dich tu mot dong cua file lich su
*
* @return true neu dong hop le (khi do object duoc cap nhat);
*         false neu sai dinh dang (object giu nguyen)
**********************************************************/
bool GiaoDich::docTuDong(const string& strDong)
{
	istringstream dongDoc(strDong);
	string strId;
	string strLoai;
	string strSoTien;
	string strIdDoiTac;
	string strTenDoiTac;
	string strThoiGian;

	bool bDocDuoc = getline(dongDoc, strId, DELIMITER)
		&& getline(dongDoc, strLoai, DELIMITER)
		&& getline(dongDoc, strSoTien, DELIMITER)
		&& getline(dongDoc, strIdDoiTac, DELIMITER)
		&& getline(dongDoc, strTenDoiTac, DELIMITER)
		&& getline(dongDoc, strThoiGian);
	if (!bDocDuoc)
	{
		return false;
	}

	// Bo ky tu '\r' neu file duoc tao tren Windows
	if (!strThoiGian.empty() && strThoiGian.back() == '\r')
	{
		strThoiGian.pop_back();
	}

	int iLoai = 0;
	long long llSoTien = 0;
	try
	{
		iLoai = stoi(strLoai);
		if (!laLoaiHopLe(iLoai))
		{
			return false;
		}
		llSoTien = stoll(strSoTien);
	}
	catch (const exception&)
	{
		return false;
	}

	this->_strId = strId;
	this->_iLoai = iLoai;
	this->_llSoTien = llSoTien;
	this->_strIdDoiTac = strIdDoiTac;
	this->_strTenDoiTac = strTenDoiTac;
	this->_strThoiGian = strThoiGian;
	return true;
}

/**********************************************************
* @Description In giao dich thanh mot dong trong bang lich su
* @attention Rut / chuyen tien hien dau "-", nhan tien hien dau "+"
**********************************************************/
void GiaoDich::hienThi() const
{
	string strSoTien = dinhDangTien(this->_llSoTien);
	switch (this->_iLoai)
	{
	case 3: // Nhan tien
		strSoTien = "+" + strSoTien;
		break;
	case 1: // Rut tien
	case 2: // Chuyen tien
		strSoTien = "-" + strSoTien;
		break;
	default:
		break;
	}

	string strDoiTac = "-";
	if (!this->_strIdDoiTac.empty())
	{
		strDoiTac = this->_strTenDoiTac + " (" + this->_strIdDoiTac + ")";
	}

	cout << left
		 << " " << setw(WIDTH_THOI_GIAN) << this->_strThoiGian
		 << setw(WIDTH_LOAI) << this->layTenLoai()
		 << right << setw(WIDTH_SO_TIEN) << strSoTien
		 << left << "  " << strDoiTac << "\n";
}

/**********************************************************
* @Description In dong tieu de cua bang lich su
**********************************************************/
void GiaoDich::hienThiTieuDeBang()
{
	cout << left
		 << " " << setw(WIDTH_THOI_GIAN) << "Thoi gian"
		 << setw(WIDTH_LOAI) << "Loai"
		 << right << setw(WIDTH_SO_TIEN) << "So tien"
		 << left << "  " << "Doi tac" << "\n";
}

/**********************************************************
* @Description Lay thoi gian he thong hien tai theo TIME_FORMAT
**********************************************************/
string GiaoDich::layThoiGianHienTai()
{
	time_t tHienTai = time(nullptr);
	tm tmHienTai = {};
#ifdef _WIN32
	localtime_s(&tmHienTai, &tHienTai);
#else
	localtime_r(&tHienTai, &tmHienTai);
#endif

	char szBuffer[TIME_BUFFER_SIZE] = { 0 };
	strftime(szBuffer, sizeof(szBuffer), TIME_FORMAT, &tmHienTai);
	return string(szBuffer);
}