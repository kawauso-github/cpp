#ifndef _B_H_
#define _B_H_
 
class A;    //  クラスAへの参照
 
class B{

private:

    A* m_pA;

public:

	// 引数付きコンストラクタ
    B(A* pA);

    void hoge();
};
 
#endif // _B_H_



