/*!
	@file      Purser.h
	@brief     各種パーサクラスを提供します。
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#ifndef _UTILITY__PURSER_H_
#define _UTILITY__PURSER_H_

namespace Utility
{
	class AbstractPurser;
	class NilPurser;
	class CharGroupPurser;
	class StringGroupPurser;
	class HexStringPurser;
}


/*!
	@brief 抽象パーサクラス(インタフェース)
*/
class Utility::AbstractPurser
{
protected:
	int m_index;

	AbstractPurser();

public:
	virtual ~AbstractPurser();

	virtual bool purse(const char* input) = 0;
	virtual int index();
};


/*!
	@brief 常に受理するパーサクラス
*/
class Utility::NilPurser : public Utility::AbstractPurser
{
public:
	NilPurser();
	virtual ~NilPurser();

	virtual bool purse(const char* input);
};


/*!
	@brief 与えられた文字グループのみ受理するパーサクラス

	@note
	CharGroupPurserは内部でリニアサーチを行うため、
	受理優先度の高い文字ほど先頭に配置すると効果的です。
*/
class Utility::CharGroupPurser : public Utility::AbstractPurser
{
private:
	const char* m_accept_chars;

public:
	CharGroupPurser(const char* accept_chars);
	virtual ~CharGroupPurser();

	virtual bool purse(const char* input);
};


/*!
	@brief 与えられた文字列グループのみ受理するパーサクラス

	@attention
	StringGroupPurserは内部でバイナリサーチを行うため、
	accept_strsには必ずソートした状態の配列を与えてください。
*/
class Utility::StringGroupPurser : public Utility::AbstractPurser
{
private:
	const char** m_accept_strs;
	const int    m_size;

public:
	StringGroupPurser(const char* accept_strs[], const int size);
	virtual ~StringGroupPurser();
	
	virtual bool purse(const char* input);
};


/*!
	@brief 16進文字列のみ受理するパーサクラス
*/
class Utility::HexStringPurser : public Utility::AbstractPurser
{
public:
	HexStringPurser();
	virtual ~HexStringPurser();

	virtual bool purse(const char* input);
};

#endif // _UTILITY__PURSER_H_