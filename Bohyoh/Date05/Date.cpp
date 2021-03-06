// 日付クラスDate（第５版：ソース部）

#include <ctime>
#include <sstream>
#include <iostream>
#include "Date.h"

using namespace std;

// 平年の各月の日数
int Date::dmax[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

//--- y年m月の日数を求める ---//
int Date::days_of_month(int y, int m)
{
	return dmax[m - 1] + (m == 2 && leap_year(y));
}

//--- year年の日数（平年…365／閏年…366）---//
int Date::days_of_year(int year)
{
	return 365 + leap_year(year);
}

//--- 調整されたm月（1〜12の範囲外の値を調整）---//
int Date::adjusted_month(int m)
{
	return m < 1 ? 1 : m > 12 ? 12 : m;
}

//--- 調整されたy年m月のd日（1〜28/29/30/31の範囲外の値を調整）---//
int Date::adjusted_day(int y, int m, int d)
{
	if (d < 1)
		return 1;

	int max_day = days_of_month(y, m);		// y年m月の日数

	return d > max_day ? max_day : d;
}

//--- デフォルトコンストラクタ（今日の日付に設定）---//
Date::Date()
{
	time_t current = time(NULL);				// 現在の暦時刻を取得
	struct tm* local = localtime(&current);		// 要素別の時刻に変換

	y = local->tm_year + 1900;		// 年：tm_yearは西暦年-1900
	m = local->tm_mon + 1;			// 月：tm_monは0〜11
	d = local->tm_mday;				// 日
}

//--- コンストラクタ（指定された年月日に設定）---//
Date::Date(int yy, int mm, int dd)
{
	set(yy, mm, dd);				// 日付をyy年mm月dd日に設定
}

//--- 年をyyに設定 ---//
void Date::set_year(int yy)
{
	y = yy;							// 年
	d = adjusted_day(y, m, d);		// 日（不正な値を調整）
}

//--- 月をmmに設定 ---//
void Date::set_month(int mm)
{
	m = adjusted_month(mm);			// 月（不正な値を調整）
	d = adjusted_day(y, m, d);		// 日（不正な値を調整）
}

//--- 日をddに設定 ---//
void Date::set_day(int dd)
{
	d = adjusted_day(y, m, dd);		// 日（不正な値を調整）
}

//--- 日付をyy年mm月dd日に設定 ---//
void Date::set(int yy, int mm, int dd)
{
	y = yy;							// 年
	m = adjusted_month(mm);			// 月（不正な値を調整）
	d = adjusted_day(y, m, dd);		// 日（不正な値を調整）
}

//--- 前日の日付を返却 ---//
Date Date::preceding_day() const
{
	Date temp(*this);				// *thisのコピーを作成
	return --temp;					// コピーの前日を求めて返却
}

//--- 翌日の日付を返却 ---//
Date Date::following_day() const
{
	Date temp(*this);				// *thisのコピーを作成
	return ++temp;					// コピーの翌日を求めて返却
}

//--- 年内の経過日数を返却 ---//
int Date::day_of_year() const
{
	int days = d;	// 年内の経過日数

	for (int i = 1; i < m; i++)			// 1月〜(m-1)月の日数を加える
		days += days_of_month(y, i);
	return days;
}

//--- 曜日を返却（日曜〜土曜が0〜6に対応）---//
int Date::day_of_week() const
{
	int yy = y;
	int mm = m;
	if (mm == 1 || mm == 2) {
		yy--;
		mm += 12;
	}
	return (yy + yy / 4 - yy / 100 + yy / 400 + (13 * mm + 8) / 5 + d) % 7;
}

//--- 文字列表現を返却 ---//
string Date::to_string() const
{
	ostringstream s;
	s << y << "年" << m << "月" << d << "日";
	return s.str();
}

//--- １日進める（前置増分：++(*this)）---//
Date& Date::operator++()
{
	if (d < days_of_month(y, m))	// 月末より前であれば
		d++;						// 　　日をインクリメントするだけ
	else {							// 翌月に繰り上がる
		if (++m > 12) {				// 　　12月を超えるのであれば
			y++;					// 　　　　　翌年の…
			m = 1;					// 　　　　　１月に繰り上がる
		}
		d = 1;						// 　　次の月の１日となる
	}
	return *this;
}

//--- １日進める（後置増分：(*this)++）---//
Date Date::operator++(int)
{
	Date temp(*this);				// インクリメント前の値をコピー
	++(*this);						// 前置増分演算子++によってインクリメント
	return temp;					// コピーを返却
}

//--- １日戻す（前置減分：--(*this)）---//
Date& Date::operator--()
{
	if (d > 1)						// 月始めでなければ
		d--;						// 　　日をデクリメントするだけ
	else {							// 前月に繰り下がる
		if (--m <= 1) {				// 　　１月を超えるのであれば
			y--;					// 　　　　　前年の…
			m = 12;					// 　　　　　12月に繰り下がる
		}
		d = days_of_month(y, m);	// 　　前月の月末となる
	}
	return *this;
}

//--- １日戻す（後置減分：(*this)--）---//
Date Date::operator--(int)
{
	Date temp(*this);				// デクリメント前の値をコピー
	--(*this);						// 前置減分演算子--によってデクリメント
	return temp;					// コピーを返却
}

//--- 日付をn日進める（複合代入：*this += n）---//
Date& Date::operator+=(int n)
{
	if (n < 0)							// nが負であれば
		return *this -= -n;			// 演算子-=に処理を委ねる

	d += n;							// 日にnを加える

	while (d > days_of_month(y, m)) {	// 日が月の日数内に収まるように年月を調整
		d -= days_of_month(y, m);
		if (++m > 12) {
			y++;
			m = 1;
		}
	}
	return *this;
}

//--- 日付のn日戻す（複合代入：*this -= int）---//
Date& Date::operator-=(int n)
{
	if (n < 0)							// nが負であれば
		return *this += -n;			// 演算子+=に処理を委ねる

	d -= n;							// 日からnを減じる

	while (d < 1) {						// 日が正になるように年月を調整
		if (--m < 1) {
			y--;
			m = 12;
		}
		d += days_of_month(y, m);
	}
	return *this;
}

//--- n日後を求める（加算：*this + n）---//
Date Date::operator+(int n) const
{
	Date temp(*this);
	return temp += n;						// 演算子+=を利用
}

//--- dayのn日後を求める（加算：n + day）---//
Date operator+(int n, const Date& day)
{
	return day + n;						// Date + intの演算子+に処理を委ねる
}

//--- dateのn日前を求める（減算：*this - n）---//
Date Date::operator-(int n) const
{
	Date temp(*this);
	return temp -= n;						// 演算子-=を利用
}

//--- 日付の差を求める（減算：*this - day）---//
long Date::operator-(const Date& day) const
{
	long count;
	long count1 = this->day_of_year();	// *thisの年内経過日数
	long count2 = day.day_of_year();	// day  の年内経過日数

	if (y == day.y)										// *thisとdayは同じ年
		count = count1 - count2;
	else if (y > day.y) {								// *thisのほうが新しい年
		count = days_of_year(day.y) - count2 + count1;
		for (int yy = day.y + 1; yy < y; yy++)
			count += days_of_year(yy);
	} else {											// *thisのほうが古い年
		count = -(days_of_year(y) - count1 + count2);
		for (int yy = y + 1; yy < day.y; yy++)
			count -= days_of_year(yy);
	}

	return count;
}

//--- dayと同じ日付か？（等価：*this == day）---//
bool Date::operator==(const Date& day) const
{
	return y == day.y && m == day.m && d == day.d;
}

//--- dayと違う日付か？（等価：*this != day）---//
bool Date::operator!=(const Date& day) const
{
	return !(*this == day);						// 演算子==を利用
}

//--- dayより後の新しい日付か？（関係：*this > day）---//
bool Date::operator>(const Date& day) const
{
	if (y > day.y) return true;		// 年が異なる（新しい）
	if (y < day.y) return false;	//     〃　　（古い）

	if (m > day.m) return true;		// 年が等しい − 月が異なる（新しい）
	if (m < day.m) return false;	//     〃　　　　　　　　　（古い）

	return d > day.d;				//     〃     − 月も等しい（日を比較）
}

//--- day以降の日付か？（関係：*this >= day）---//
bool Date::operator>=(const Date& day) const
{
	return !(*this < day);						// 演算子<を利用
}

//--- dayより前の古い日付か？（関係：*this < day）---//
bool Date::operator<(const Date& day) const
{
	if (y < day.y) return true;		// 年が異なる（古い）
	if (y > day.y) return false;	//     〃    （新しい）

	if (m < day.m) return true;		// 年が等しい − 月が異なる（古い）
	if (m > day.m) return false;	//     〃　　　　　　　　　（新しい）

	return d < day.d;				//     〃     − 月も等しい（日を比較）
}

//--- day以前の日付か？（関係：*this <= day）---//
bool Date::operator<=(const Date& day) const
{
	return !(*this > day);						// 演算子>を利用
}

//--- 出力ストリームsに日付xを挿入 ---//
ostream& operator<<(ostream& s, const Date& x)
{
	return s << x.to_string();
}

//--- 入力ストリームsから日付を抽出してxに格納 ---//
istream& operator>>(istream& s, Date& x)
{
	int yy, mm, dd;
	char ch;

	s >> yy >> ch >> mm >> ch >> dd;
	x = Date(yy, mm, dd);
	return s;
}