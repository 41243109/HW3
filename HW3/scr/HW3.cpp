#include <iostream>
#include <cmath>
#include <limits>
using namespace std;

//節點類別，表示多項式的單項
class Node
{
public:
    float coef;  //係數
    int exp;  //指數
    Node* link;  //指向下一個節點的連結

    //初始化係數、指數與鏈結
    Node(float c = 0, int e = 0, Node* l = nullptr)
        : coef(c), exp(e), link(l) {}
};

//多項式類別
class Polynomial
{
private:
    Node* head;  //頭節點，作為循環鏈結串列的起點

    //將新的節點加到鏈結最後面
    void Attach(float coef, int exp, Node*& tail)
    {
        if (abs(coef) < 1e-6) return;  //係數接近0，則忽略該項
        Node* newNode = new Node(coef, exp);
        tail->link = newNode;  //將最後節點的鏈結指向新節點
        tail = newNode;  //更新最後節點為新節點
    }

    //清除輸入緩衝區
    void clearInputBuffer()
    {
        cin.clear();  //清除輸入的錯誤狀態
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  //忽略緩衝區中的內容直到換行符
    }

public:
    //建構子
    Polynomial() {
        head = new Node();
        head->link = head;
    }

    // 解構函子
    ~Polynomial() {
        Node* curr = head->link;
        while (curr != head)
        {
            Node* temp = curr;   //儲存當前節點
            curr = curr->link;
            delete temp;  //刪除節點內容
        }
        delete head;
    }

    // 複製建構函數
    Polynomial(const Polynomial& other)
    {
        head = new Node();
        head->link = head;
        Node* tail = head;
        Node* curr = other.head->link;
        while (curr != other.head)
        {
            Attach(curr->coef, curr->exp, tail);  //將當前節點複製並加到新多項式
            curr = curr->link;
        }
        tail->link = head;  //關閉循環鏈結串列
    }

    //運算子
    Polynomial& operator=(const Polynomial& other) {
        if (this != &other)  //防止自我賦值
        {
            Node* curr = head->link;
            while (curr != head)
            {
                Node* temp = curr;
                curr = curr->link;
                delete temp;
            }

            // 複製另一個多項式的數據
            Node* tail = head;  //最後節點為最前節點
            curr = other.head->link;  //從另一個多項式的第一個節點開始
            while (curr != other.head)
            {
                Attach(curr->coef, curr->exp, tail);  //將當前節點複製加入到新多項式
                curr = curr->link;
            }
            tail->link = head;  //關閉循環環鏈結串列
        }
        return *this;  //返回當前引用
    }

    // 輸入運算子
    friend istream& operator>>(istream& is, Polynomial& x)
    {
        int n;
        do
        {
            cout << "輸入多項式的項數（正整數）：";
            if (!(is >> n) || n <= 0)  //檢查輸入是不是有效的正整數
            {
                cout << "請輸入有效的正整數！" << endl;
                x.clearInputBuffer();  //清除輸入緩衝區
                continue;
            }
            break;  //輸入有效就退出循環
        } while (true);

        Node* tail = x.head;  //最後節點為最前節點
        int lastExp = INT_MAX;  //用於檢查指數是不是較小，初始設為最大值

        for (int i = 0; i < n; ++i)
        {
            float coef;  //儲存係數
            int exp;  //儲存指數
            bool validInput = false;  //檢查輸入是否有效

            do
            {
                cout << "輸入第" << (i + 1) << "項的係數與指數 (例如 3 2 表示 3x^2)：";
                if (!(is >> coef >> exp))  //檢查輸入是不是有效的數值
                {
                    cout << "輸入無效，請重試！" << endl; 
                    x.clearInputBuffer();
                    continue;
                }

                if (exp >= lastExp)  //檢查指數是否比前面的還小
                {
                    cout << "指數必須小於前一項！前一項指數為：" << lastExp << endl;
                    continue;
                }

                validInput = true; //輸入有效就退出循環
                lastExp = exp;  //更新最後的指數
            } while (!validInput);

            if (abs(coef) >= 1e-6)  //忽略係數為0的項
            {
                x.Attach(coef, exp, tail);  //將有效項加到多項式
            }
        }
        tail->link = x.head;  //關閉合循環鏈結串列
        return is;
    }

    // 輸出運算子，將多項式輸出到輸出流
    friend ostream& operator<<(ostream& os, const Polynomial& x)
    {
        Node* curr = x.head->link;  //從第一個有效節點開始
        if (curr == x.head)  //如果多項式為空
        {
            os << "0";
            return os;
        }

        bool first = true;  //用於標記是否為第一項
        while (curr != x.head)
        {
            if (curr->coef == 0)  //忽略係數為0的項
            {
                curr = curr->link;
                continue;  //跳過本次迭代
            }

            if (!first && curr->coef > 0) os << "+";  //如果非第一項且係數為正，則輸出+
            first = false;  //設置標記為非第一項

            if (curr->coef != 1 || curr->exp == 0)  //係數不為1或指數為0
            {
                if (curr->coef == -1 && curr->exp != 0)  //係數為-1且指數不為0
                {
                    os << "-";
                }
                else {
                    os << curr->coef;
                }
            }

            if (curr->exp > 0)  //如果指數大於0
            {
                os << "x";
                if (curr->exp > 1) os << "^" << curr->exp;  //如果指數大於1則輸出指數
            }

            curr = curr->link;
        }
        return os;
    }

    // 加法運算子
    Polynomial operator+(const Polynomial& b) const
    {
        Polynomial result;  //用於存儲結果的多項式
        Node* aPtr = head->link;
        Node* bPtr = b.head->link;
        Node* tail = result.head;

        while (aPtr != head && bPtr != b.head)  //同時跑兩個
        {
            if (aPtr->exp == bPtr->exp)  //如果指數相等
            {
                float sum = aPtr->coef + bPtr->coef;  //計算係數和
                if (abs(sum) >= 1e-6)  //若和不為0
                {
                    result.Attach(sum, aPtr->exp, tail);  //將和附加到結果多項式
                }
                aPtr = aPtr->link;
                bPtr = bPtr->link;
            }
            else if (aPtr->exp > bPtr->exp)  //如果a多項式指數較大
            {
                result.Attach(aPtr->coef, aPtr->exp, tail); // 加入a多項式的當前項
                aPtr = aPtr->link;
            }
            else  //如果b多項式指數較大
            {
                result.Attach(bPtr->coef, bPtr->exp, tail); // 加入b多項式的當前項
                bPtr = bPtr->link;
            }
        }

        while (aPtr != head)  //若a還有剩餘項
        {
            result.Attach(aPtr->coef, aPtr->exp, tail);  //加到結果多項式
            aPtr = aPtr->link;
        }

        while (bPtr != b.head)  //若b還有剩餘項
        {
            result.Attach(bPtr->coef, bPtr->exp, tail);  //加到結果多項式
            bPtr = bPtr->link;
        }

        tail->link = result.head;  //關閉結果多項式鏈結串列
        return result;
    }

    // 計算多項式的值，給定x
    float Evaluate(float x) const {
        float result = 0;
        Node* curr = head->link;
        while (curr != head)
        {
            result += curr->coef * pow(x, curr->exp);  //累加每個數值
            curr = curr->link;
        }
        return result;
    }
};

// 主函數，測試多項式類別的功能
int main() {
    try {
        Polynomial p1, p2;
        cout << "輸入A多項式" << endl;
        cin >> p1;
        cout << "輸入B多項式" << endl;
        cin >> p2;

        cout << "\nA多項式為: " << p1 << endl;
        cout << "B多項式為: " << p2 << endl;

        Polynomial sum = p1 + p2;
        cout << "加法結果: " << sum << endl;

        float x; // 用於存儲用戶輸入的x值
        do {
            cout << "\n輸入X值計算A多項式結果: ";
            if (!(cin >> x))
            {
                cout << "請輸入有效的數值！" << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');  //忽略緩衝區中的內容直到換行符號
                continue;
            }
            break;  //若輸入有效就退出循環
        } while (true);

        cout << "A(" << x << ") = " << p1.Evaluate(x) << endl;
    }
    catch (const exception& e)
    {
        cerr << "發生錯誤：" << e.what() << endl;
        return 1;
    }

    return 0;
}
