#include <iostream>

// class Point {
// public:
//     // 获取单例实例（注意：返回指针）
//     static Point* getInstance() {
//         if (_pInstance == nullptr) {
//             _pInstance = new Point();  // 第一次创建
//         }
//         return _pInstance;
//     }

//     // 设置坐标
//     void set(double newX, double newY) {
//         _pInstance->x = newX;
//         _pInstance->y = newY;
//     }

//     // 显示坐标
//     void print() const {
//         std::cout << "(" << _pInstance->x << ", " << _pInstance->y << ")\n";
//     }

//     // 提供手动销毁接口
//     static void destroyInstance() {
//         if (_pInstance) {
//             delete _pInstance;
//             _pInstance = nullptr;
//         }
//     }

// private:
//     double x = 0.0;
//     double y = 0.0;

//     // 构造私有，禁止外部 new
//     Point() {
//         std::cout << "Point() 构造\n";
//     }

//     ~Point() {
//         std::cout << "Point() 析构\n";
//     }

//     // 禁止拷贝和赋值
//     Point(const Point&) = delete;
//     Point& operator=(const Point&) = delete;

//     // 私有 static 成员变量
//     static Point* _pInstance;
// };

// // static 成员变量定义（初始化为 nullptr）
// Point* Point::_pInstance = nullptr;

using namespace std;
void printStreamStatus(istream & is)
{
    cout << "is.good() = " << is.good() << endl;
    cout << "is.bad() = " << is.bad() << endl;
    cout << "is.fail() = " << is.fail() << endl;
    cout << "is.eof() = " << is.eof() << endl;
}
void test2()
{
    int value;
    printStreamStatus(cin);
    while(cin >> value) {
        cout << value << endl;
    }
    printStreamStatus(cin);
    std::cin.clear();
    printStreamStatus(cin);
    cin.ignore(1024, '\n');
    std::string s;
    std::cin >> s;
    std::cout << s << std::endl;
}



int main() {
    test2();
    // Point* p1 = Point::getInstance();
    // // Point p1 =  Point();
    // p1->set(10, 20);
    // p1->print();  // 输出: (10, 20)

    // Point* p2 = Point::getInstance();
    // p2->print();  // 依然是 (10, 20)

    // std::cout << "是否同一对象: " << std::boolalpha << (p1 == p2) << '\n';

    // Point::destroyInstance();  // ✅ 手动释放内存



    return 0;
}
