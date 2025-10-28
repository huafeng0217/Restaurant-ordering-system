#include <iostream>
#include<string>
#include<memory>
#include<vector>
#include<map>
#include<algorithm>
#include<fstream>
#include<sstream>
#include<chrono>
#include<iomanip>
#include <limits>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

// 设置控制台编码的函数
void setConsoleUTF8() {
#ifdef _WIN32
    // 设置控制台输出为UTF-8
    SetConsoleOutputCP(65001);
    // 设置控制台输入为UTF-8
    SetConsoleCP(65001);
#endif
}


class Dish{


public:
    int id;// 食物编号
    string name;// 食物名字
    string kind;// 食物种类
    double price;// 食物价格
    int stock;// 食物库存
    string description;// 食物描述

    Dish(string name,int id,string kind,double price,int stock,string description) {
        this->name=name;
        this->id=id;
        this->kind=kind;
        this->price=price;
        this->stock=stock;
        this->description=description;
    }
    ~Dish(){}
    string getname() {// 取得食物名字
        return this->name;
    }
    string getkind() {// 取得食物种类
        return this->kind;
    }
    double getprice() {// 取得食物价格
        return this->price;
    }
    int getstock() {// 取得食物库存数量
        return this->stock;
    }
    string getdescription() {// 取得食物描述
        return this->description;
    }
    void setname(string name) {// 修改食物名字
        this->name=name;
        cout<<"修改成功!"<<endl;
    }
    void setkind(string kind) {// 修改食物种类
        this->kind=kind;
        cout<<"修改成功"<<endl;
    }
    void setprice(double price) {// 修改食物价格
        this->price=price;
        cout<<"修改成功"<<endl;
    }
    void setstock(int stock) {// 修改食物库存
        this->stock=stock;
        cout<<"修改成功"<<endl;
    }
    void setdescription(string description) {// 修改食物描述
        this->description=description;
        cout<<"修改成功"<<endl;
    }

};



struct Trienode {
    char node;// 结点字符
    map<char,int>child;// 结点孩子字符内容和地址编号
    bool flag=false;// 标签是否为一个字符串的尾结点
    int childcount=0;// 结点孩子数
   Dish* relatedfood=nullptr;// 以该节点结尾的食物
};

const int N=1000;// 分配给的Trie数结点个数
class Trie {
public:
    Trie() {// 析构函数
        pt[0]=Trienode();// 初始化根结点
    }
    ~Trie(){}

    // 属性
    Trienode pt[N];// 结点集
    int cnt=0;// 总结点个数,通过他的增加可以为新结点编号

    // 成员方法
    void insert(string s,Dish* dish) {// 添加食物到Trie树中
       int slen=s.length(),pos=0;
        for (char c:s)
        {
           if (pt[pos].child.find(c)==pt[pos].child.end())// 在孩子结点集中寻找字符，如果没找到，返回的是map.end()
           {
               pt[pos].child[c]=++cnt;// 创建了一个新结点
               pt[pos].childcount++;// 当前pos指向的结点孩子结点数量增加
           }
            pos=pt[pos].child[c];
        }
        pt[pos].flag=true;
        pt[pos].relatedfood=dish;

    }

    vector<Dish*>searchfood(string prefix) {
        vector<Dish*> results;// 用于储存查找的结果
        int pos=0;
        for (char c:prefix) {
            if(pt[pos].child.find(c)==pt[pos].child.end())
               return results;// 当有一个字符没找到，就放回空结果
            pos=pt[pos].child[c];
        }
        dfsfood(pos,results);
       return results;
    }
private:
    void dfsfood(int now_pos,vector<Dish*>&results) {
        results.push_back(pt[now_pos].relatedfood);
        for (auto&pair:pt[now_pos].child) {
            dfsfood(pair.second,results);// 将字符下的孩子结点的地址编号来dfs查找相关食物
        }
    }
};

class Menumanager {
// 菜单管理(食物管理)
private:
    vector<Dish>food;// 主储存所有食物
    map<string,vector<Dish*>>category;// 储存的是各种食物的分类和食物集
    // 这里类别采用*的原因是使food中储存的是对象原件，然后食物类别中储存的是Dish指针指向原件地址
    // 这样避免了重复创建副本，浪费空间;
    // 且对对象相关属性进行修改时不需要修改多次，保持一致性
    Trie foodtree;// 食物搜索的trie树
    int nextid;// 食物的id生成
public:
    Menumanager() {
        nextid=0;
    }
    ~Menumanager() {}

    //食物修改操作
    bool addfood(string name,string kind,double price,int stock,string description) {//添加食物
        if (name.empty())cout<<"食物名称不能为空!"<<endl;
        if (kind.empty()) cout<<"食物种类不能为空!"<<endl;
        if (price<0)cout<<"食物价格不能小于0!"<<endl;
        if (stock<0)cout<<"食物库存不能为0!"<<endl;
        if (description.empty())cout<<"食物描述不能为空!"<<endl;
        if (isfoodexit(name))cout<<"食物已存在!"<<endl;

        else {
            Dish newfood(name,nextid,kind,price,stock,description);// 调用了Dish的构造函数
            food.push_back(newfood);
            Dish* newfoodptr=&food.back();
            category[kind].push_back(newfoodptr);//这里一开始犯了错误就是将newfood指针直接存入category
            //但是实际上将newfood指针存入到food中，food中是创建了一个副本拷贝了newfood,newfood是临时变量，出了这个函数就会销毁
            //所以category应该存储的是food中新添加的副本
            foodtree.insert(name,newfoodptr);
            this->nextid++;
            cout<<"食物"<<name<<"添加成功"<<endl;
        }
    }
    bool removefood(int id) {// 通过id来移除食物
        int foodid=-1;
        string foodkind="";
        for (auto&c:food) {
            if (c.id==id){
                foodid=c.id;foodkind=c.kind;break;
            }
        }
        if (foodid==-1){cout<<"错误：未找到食物"<<endl;return false;
        }// 没有该食物
        if (category.find(foodkind)==category.end()) {
            cout << "错误：分类 '" << foodkind<< "' 不存在！" << endl;
            return false;
        }
        else {
            return remove(foodid,foodkind);
        }
    }
    bool removefood(string name) {// 通过名字来移除食物
        // 通过id来移除食物
        int foodid=-1;
        string foodkind="";
        for (auto&c:food) {
            if (c.name==name){
                foodid=c.id;foodkind=c.kind;break;
            }
        }
        if (foodid==-1){cout<<"错误：未找到食物"<<endl;return false;
        }// 没有该食物
        if (category.find(foodkind)==category.end()) {
            cout << "错误：分类 '" << foodkind<< "' 不存在！" << endl;
            return false;
        }
        else {
            return remove(foodid,foodkind);
        }
    }
    bool updatefood(int foodid,string newname,string newkind,double newprice,int newstock,string newdescription) {//更新食物信息
        Dish* target=nullptr;
        for(auto&c:food)
            if (c.id==foodid){target=&c;break;}
        if (target==nullptr){cout<< "食物不存在"<<endl;return false;}
        if(newname == target->name||isfoodexit(newname)){cout<<"食物新名称与现有食物重复"<<endl;return false;}
        string oldname=target->name;// 记录下旧的食物名称和种类用于删除操作
        string oldkind=target->kind;
        target->name=newname;
        target->kind=newkind;
        target->price=newprice;
        target->description=newdescription;
        if(oldkind!=newkind){
            vector<Dish*>it=(category.find(oldkind))->second;
            it.erase(
            remove_if(it.begin(),it.end(),[target](Dish*d){
              return d->id==target->id;
        }),it.end());
            if(it.empty()){category.erase(category.find(oldkind));}
            category[newkind].push_back(target);
            cout<<"食物信息更新成功"<<endl;
        };

    }

    //食物信息获取工作
    vector<Dish*>searchdishes(string prefix){//查找相关菜品
        vector<Dish*>results=foodtree.searchfood(prefix);
        cout<<"找到"<<results.size()<<"相关菜品"<<endl;
        return results;
    }


    Dish* getdish(string name)//通过名字找到菜品
    {
        for(auto&c:food)
        {   if(c.name==name)return &c;
        }
        return nullptr;
    }

    Dish* getdish(int id)//通过id寻找菜品
    {
        for(auto&c:food)
        {   if(c.id==id)return &c;}
        return nullptr;
    }
    vector<Dish>getallfood()//查找所有事物
    {
        return food;
    }
    vector<Dish*>getdishbycategory(string kind)// 查找一个类的所有食物
    {
        for(auto&c:category){
            if(c.first==kind)return c.second;
        }
    }

    // 分类管理
    vector<string>getallcategory(){//查看所有类
        vector<string>result;
        for(auto&c:category){
            result.push_back(c.first);}
        return result;
    }

    bool iskindexit(string kind)//查看一个类是否存在
    {     bool flag=false;
        for(auto&c:category){
            if(c.first==kind){flag=true;break;}
        }
        return flag;
    }

    int getnumberinonekind(string kind)//查找一个类中的菜品数量
    {
        return category.find(kind)->second.size();
    }

    //文件操作
    bool savetofile(string filename="food.txt")// 将食物信息储存至文件
    {
        ofstream fout;
        fout.open(filename,ios::out);// 以ios::out方式打开文件，即输入的内容会覆盖原文件内容
        if(!fout.is_open()){
            cout<<"打开文件"<<filename<<"失败"<<endl;
            return false;
        }
        fout<<nextid<<endl;
        for(const auto&c:food)
        {
            fout<<c.name<<","<<c.id<<","<<c.kind<<","<<c.price<<","<<c.stock<<","<<c.description<<endl;
        }
        fout.close();
        cout<<"食物信息储存至"<<filename<<"成功"<<endl;
        return true;
    }
    bool loadfromfile(string filename="food.txt") {
        ifstream fin;
        int loadcount=0;
        fin.open(filename,ios::in);
        if(!fin.is_open()){cout<<"打开文件"<<filename<<"失败"<<endl;
            return false;}
        food.clear();
        category.clear();
        string line;
        getline(fin,line);
        nextid=stoi(line);// 最开始的一行储存的是id生成器
        while (getline(fin,line)){
            string name,idstr,kind,pricestr,stockstr,description;
            if (line.empty())continue;
            stringstream ss(line);// stringstream配合getline是用来将文件的每行字符串进行分割，string无法做到
            getline(ss,name,',');// 将ss中第一个逗号前的内容分割出来给到name
            getline(ss,idstr,',');// 这里是第二个逗号，因为读取文件内容时会有一个读取指针，从指针后开始读取
            getline(ss,kind,',');
            getline(ss,pricestr,',');
            getline(ss,stockstr,',');
            getline(ss,description,',');
            try {//try catch是进行错误检测，如果try中有一个语句出现错误，无法实现，则直接跳转到catch执行后面语句
                //类型转换
                int id = stoi(idstr);        // string → int
                double price = stod(pricestr); // string → double
                int stock = stoi(stockstr);   // string → int

                //创建菜品对象
                Dish dish(name, id, kind, price, stock, description);
                food.push_back(dish);

                //更新索引
                Dish* dishptr = &food.back();
                foodtree.insert(name, dishptr);          // 添加到Trie树
                category.find(kind)->second.push_back(dishptr);        // 更新分类索引
                loadcount++;
            } catch (const exception& e) {
                cout << "警告：解析菜品数据失败: " << line << endl;
                cout << "错误信息: " << e.what() << endl;
            }
        }

        // 关闭文件
        fin.close();
        cout << "✅ 从 " << filename << " 加载了 " << loadcount << " 个菜品" << endl;
        return true;
    }
    //食物数据展示
    int getallfoodnum(){//食物总数量
        return food.size();
}
    int getallcategorynum() {//食物种类数量
        return category.size();
    }

    void displaymenudate() {//基本菜单数字信息
        cout << "\n=== 菜单统计 ===" << endl;
        cout << "总菜品数量: " << getallfoodnum() << endl;
        cout << "分类数量: " << getallcategorynum() << endl;

        cout << "\n各分类菜品数量:" << endl;
        for (const auto& kind : getallcategory()) {
            int count = getnumberinonekind(kind);
            cout << "  " << kind << ": " << count << " 个菜品" << endl;
        }
    }



private:
    bool remove(int foodid,const string& foodkind) {//移除食物的辅助函数
        auto it=category.find(foodkind);
        if (it!=category.end()) {
            vector<Dish*>&target=it->second;

            target.erase(
                   remove_if(target.begin(), target.end(),
                       [foodid](Dish* dish) {
                           return dish->id == foodid;
                       }),
                   target.end()
               );

            // 如果分类为空，删除整个分类
            if (target.empty()) {
                category.erase(it);
            }


            // 从主存储food中删除
            food.erase(
                remove_if(food.begin(), food.end(),
                    [foodid](const Dish& dish) {
                        return dish.id == foodid;
                    }),
                food.end()
            );
        }
    cout << "✅ 成功删除食物 (ID: " << foodid << ")" << endl;
    return true;
        /*
           remove_if算法函数用于从容器中移除满足特定条件的元素，作用：遍历容器从first到last的范围，
           移动所有不满足条件的元素到容器前面，返回新的逻辑结尾的迭代器，不会改变容器实际大小，需要配合erase使用
           eg.一个容器vector<int>a={1,2,3,4,5,6}
              条件为（x%2==0)的要删除
              使用remove_if后会将不符合条件的元素向前移，不符合的向后移动
              返回一个不符合元素开始的迭代器可以当作指针
              然后erase(该迭代器，容器结尾）就可以删除
           使用方法：remove_if(a.begin(),a.end(),[这里用于传入参数，无参数则不传入](int n临时变量){return n==2});
            */
}
    //检查该食物是否已在菜单中
    bool isfoodexit(string name) {
        for(auto&c:food) {
            if (c.name==name)return true;
        }
        return false;
    }

 };



//订单类
enum class orderstatus {//订单状态枚举，跟c的enum类似但是更加安全
    Pending,//待处理
    Preparing,//制作中
    Completed,//已完成
    Canceled//已取消
};
class Order {
private:
    int orderid;//订单号
    int tablenumber;//订单桌
    orderstatus status;//订单状态
    chrono::system_clock::time_point createtime;//这是c++的现代时间库chrono定义了一个时间点变量，保存订单的创建时间
    map<int,pair<Dish*,int>>orderedfood;//储存点的菜品号，菜品以及数量
public:
    Order(int id,int tablenum) {
        this->orderid=id;
        this->tablenumber=tablenum;
        createtime=chrono::system_clock::now();
        status=orderstatus::Pending;//创建订单时初始化订单为待处理状态
    }
    ~Order(){}
    bool addfood(Dish* c,int number) {
        if (!c) {
            cout<<"菜单中该食物不存在"<<endl;
            return false;
        }
        if (number<=0) {
            cout<<"食物份量不能小于0"<<endl;
            return false;
        }
        if (number>c->stock) {
            cout<<"所点食物份量太多，库存不足"<<endl;
            cout<<"当前库存数为："<<c->stock<<endl;
            return false;
        }
        //添加或更新菜品数量
        if (orderedfood.find(c->id)!=orderedfood.end()){//这里有一个发现了自己之前的一个误区：map容器的map[first]是索引到second的
            //而自己之前一直以为是索引到first和second整个元素；
            //只有map.find(c->id)才是索引到整个元素组;
            orderedfood[c->id].second+=number;//增加菜品数量
        }
        else {
            //新添加菜品
            orderedfood[c->id]=make_pair(c,number);
        }
        cout<<"已添加"<<number<<"份"<<c->name<<"到订单"<<endl;
        return true;
    }

    bool removefood(Dish* c) {//通过Dish指针移除菜品
        if (status != orderstatus::Pending) {
            cout<<"该菜品正在制作中,无法修改！"<<endl;
            return false;
        }
        if (orderedfood.find(c->id)==orderedfood.end()) {
            cout<<"订单中没有该菜品，请确认您是否下单成功"<<endl;
            return false;
        }
        auto it=orderedfood.find(c->id);
        orderedfood.erase(it);
        cout<<"菜品"<<c->name<<"已删除"<<endl;
        return true;
    }
    bool removefood(int id) {//通过Dishid移除菜品
        auto it=orderedfood.find(id);
        if (it==orderedfood.end()) {
            cout<<"订单中没有该菜品，请确认您是否下单成功"<<endl;
            return false;
        }
        if (status != orderstatus::Pending) {
            cout<<"该菜品正在制作中,无法修改！"<<endl;
            return false;
        }
        cout<<"菜品"<<it->second.first->name<<"已删除"<<endl;
        orderedfood.erase(it);
        return true;

    }
    void clearorder() {//订单清空方法，ordermanager使用
        orderedfood.clear();
    }
    bool isempty() {//检查是否为空的方法
        return orderedfood.empty();
    }
    bool updatestock() {//更新库存中的菜品数量
        if (status ==orderstatus::Preparing) {//当前订单状态为制作中才需要减少库存，因为此时订单已经不能取消了。
            for (const auto& c:orderedfood) {
                auto it=c.second.first;
                it->stock-=c.second.second;
            }
            cout<<"更新库存成功"<<endl;
            return true;
        }
        cout<<"该状态下的订单无需更新库存"<<endl;
        return false;
    }
    double calaulatetotal() {
        //计算总价格
        double total=0.0;
        for (const auto& c:orderedfood) {
            auto it=c.second.first;
            total+=it->price*c.second.second;
        }
        return total;
    }
    orderstatus getstatus() {//获取status,在后面的ordermanager中使用
        return status;
    }
    void setstatus(orderstatus status) {//设置status,orderstatus使用,后面的管理系统都使用该转变订单状态方法，因为只有这个函数才有更新库存功能
        this->status = status;
        updatestock();
    }

    void setpreparing() {//设置订单状态为准备中
            updatestock();
            status=orderstatus::Preparing;
            cout<<"订单状态变化，菜品正在准备中请您耐心等待"<<endl;

    }
    void setcompleted() {//设置订单状态为已完成
            status=orderstatus::Completed;
            cout<<"订单状态变化：您的订单已完成，感谢您的品尝"<<endl;

    }
    void setcanceled() {//设置订单状态为取消
        status=orderstatus::Canceled;
        orderedfood.clear();
        cout<<"订单状态变化：订单已被取消"<<endl;
    }
    void statuscheck() {//查看订单状态
        if (status==orderstatus::Pending) {cout<<"您的订单正在等候处理中"<<endl;}
        else if (status==orderstatus::Preparing) {cout<<"您的菜品正在制作中，请您耐心等待"<<endl;}
        else if (status==orderstatus::Completed){cout<<"您所点菜品已经全部上齐，订单已完成"<<endl;}
        else{cout<<"您的订单已取消"<<endl;}
    }
    int getorderid() {//获取订单号
        return orderid;
    }
    int gettablenumber() {//获取订单桌牌号
        return tablenumber;
    }
    string gettime() {//获取时间
       return timetostring(createtime);
    }
    const map<int,pair<Dish*,int>>&getorderedfood() {
        return orderedfood;
    }

    void displayorder() {
        cout << "\n=================================" << endl;
        cout << "           订单详情" << endl;
        cout << "=================================" << endl;
        cout << "订单号: " << orderid << " | 桌号: " << tablenumber << endl;
        cout << "下单时间: " << gettime() << endl;
        cout << "---------------------------------" << endl;
        cout << "菜品清单:" << endl;
        cout << "---------------------------------" << endl;

        if (orderedfood.empty()) {
            cout << "   订单为空" << endl;
        } else {
            int index = 1;
            for (const auto& c : orderedfood) {
                Dish* dish = c.second.first;
                int quantity = c.second.second;
                double subtotal = dish->price * quantity;

                cout << "   " << index++ << ". " << dish->name
                     << " × " << quantity<<"   "
                     << "￥" << subtotal << endl;
            }
        }

        cout << "---------------------------------" << endl;
        cout << "总计: ￥" << calaulatetotal() << " 元" << endl;
        cout << "状态: ";
        statuscheck();
        cout << "=================================" << endl;
    }




private:string timetostring(chrono::system_clock::time_point time) {

    time_t t=chrono::system_clock::to_time_t(time);//time为C++的现代时间类型，现在转换为C的传统时间类型time_t，也叫c风格时间戳
    tm t1;
    localtime_s(&t1,&t);//这里是将t时间进行分解为本地时间的分解结构
    /*tm结构体包含：struct tm {
    int tm_sec;   // 秒 [0-59]
    int tm_min;   // 分 [0-59]
    int tm_hour;  // 时 [0-23]
    int tm_mday;  // 日 [1-31]
    int tm_mon;   // 月 [0-11] （0=1月）
    int tm_year;  // 年（从1900开始）
    int tm_wday;  // 星期 [0-6]（0=周日）
     ... 其他字段*/
    stringstream ss;
    ss<<put_time(&t1,"%Y-%m-%d %H:%M:%S");
    //put_time函数是将tm*时间结构格式化为指定的字符串格式
    //put_time(const tm* tmb,const char* fmt);
    //fmt有：%Y,4位数年份；%y,2位数年份；%m,2位数月份；%H,24小时制小时；%M，分钟；%S,秒钟；
    return ss.str();
};
};

//订单管理类
class Ordermanager {
private:
    vector<Order>orders;//储存所有订单的容器
    Menumanager* menumanager;//菜单管理器指针
    int nextid;//订单ID生成器
public:
    Ordermanager(Menumanager* menumanager) {
        this->menumanager=menumanager;
        nextid=1;
    }
    ~Ordermanager() {}
    //订单管理
    Order* createorder(int tablenumber) {//创建订单
        orders.emplace_back(nextid,tablenumber);//emplace_back()直接在vector中构造，push_back拷贝构造
        Order* neworder=&orders.back();
        nextid++;
        cout<<"创建新订单，订单ID:"<<neworder->getorderid()<<",桌号："<<neworder->gettablenumber()<<endl;
        return neworder;
    }
    Order* getorder(int id) {//查找订单
        for (auto& order:orders) {
            if (order.getorderid()==id)return &order;
        }
        return nullptr;
    }
    bool deleteorder(int id) {
        if (getorder(id)==nullptr) {
            cout<<"订单不存在"<<endl;
            return false;
        }
        else {
            /*
             这里犯了一个错误：
             一开始自己想要用getorder(id)来获取订单的指针然后用orders的erase来实现订单的删除
             但这样子是不对的，vector容器的erase()是要配合其迭代器来使用的，迭代器我之前只将其浅显的理解为普通指针的替换
             实际上迭代器可以算是一个智能指针，包含了一个元素在vector中的具体位置，让vector知道如何移动后续元素和内部大小信息
             而指针就只是指向了一块内存地址，无法传递更多的信息
             eg.下面代码的it->getorderid()==(*it)->getorderid()这里就是迭代器的重载的*运算符了使得it可以直接使用成员函数
                而不是通过一个指针的解引用
                */

            for(auto it=orders.begin();it!=orders.end();it++) {
                if (it->getorderid()==id){
                    orders.erase(it);
                    cout<<"成功删除订单："<<id<<endl;
                    return true;
                }
            }
        }
    }

    //订单操作
    bool addfoodtoorder(int orderid,int dishid,int quantity=1) {//将菜加入订单中
        if (getorder(orderid)==nullptr) {cout<<"该订单号不存在"<<endl;return false;}
        if (menumanager->getdish(dishid)==nullptr){cout<<"该菜品不存在"<<endl;return false;}
        return getorder(orderid)->addfood(menumanager->getdish(dishid),quantity);

    }
    bool removefoodfromorder(int orderid,int dishid) {//从订单中移除菜品
        if (!getorder(orderid)){cout<<"该订单号不存在"<<endl;return false;}
        if (!menumanager->getdish(dishid)){cout<<"该菜品不存在"<<endl;return false;}
        return getorder(orderid)->removefood(menumanager->getdish(dishid));

    }
    bool updatefoodinorder(int orderid,int dishid,int quantity) {
        if (!getorder(orderid)){cout<<"该订单号不存在"<<endl;return false;}
        if (!menumanager->getdish(dishid)){cout<<"该菜品不存在"<<endl;return false;}
        getorder(orderid)->removefood(menumanager->getdish(dishid));//先移除食物（食物数量清0）
        return getorder(orderid)->addfood(menumanager->getdish(dishid),quantity);//再增加食物
    }

    //订单状态操作
    bool setorderpreparing(int orderid) {//订单状态设置为准备中
        if (!getorder(orderid)) {
            cout<<"该订单后不存在"<<endl;
            return false;
        }
        getorder(orderid)->setpreparing();
        return true;
    }
    bool setorderompleted(int orderid) {//订单状态设置为已完成
        if (!getorder(orderid)) {
            cout<<"该订单后不存在"<<endl;
            return false;
        }
         getorder(orderid)->setcompleted();
        return true;

    }
    bool setordercanceled(int orderid) {//订单状态设置为被取消
        if (!getorder(orderid)) {
            cout<<"该订单后不存在"<<endl;
            return false;
        }
        getorder(orderid)->setcanceled();
        return true;
    }

    //订单查询
    vector<Order*> getordersbystatus(orderstatus status) {//按照订单状态查询订单
        vector<Order*> results;
        for (auto &c:orders) {
            if (c.getstatus()==status) {results.push_back(&c);}
        }
        return results;
    }
    vector<Order*>getordersbyorderid(int orderid) {//按照桌号查找订单
        vector<Order*> results;
        for (auto &c:orders) {
            if (c.getorderid()==orderid) {results.push_back(&c);}
        }
        return results;
    }
    vector<Order*>getallorders() {//获取所有订单
        vector<Order*> results;
        for (auto &c:orders) {
            results.push_back(&c);
        }
        return results;
    }
    vector<Order*>getpendingorders() {//获取待处理的订单
        vector<Order*> results;
        for (auto &c:orders) {
            if (c.getstatus()==orderstatus::Pending) {
                results.push_back(&c);
            }
        }
        return results;
    }
    vector<Order*>getpreparingorders() {//获取准备中的订单
        vector<Order*> results;
        for (auto &c:orders) {
            if (c.getstatus()==orderstatus::Pending) {results.push_back(&c);}
        }
        return results;
    }
    vector<Order*>getcompletedorders() {//获取已完成的订单
        vector<Order*> results;
        for (auto &c:orders) {
            if (c.getstatus()==orderstatus::Completed) {results.push_back(&c);}
        }
        return results;
    }
    vector<Order*>getcanceledorders() {//获取被取消的订单
        vector<Order*> results;
        for (auto &c:orders) {
            if (c.getstatus()==orderstatus::Canceled) {results.push_back(&c);}
        }
        return results;
    }
    vector<Order*>getordersbytable(int tablenumber) {//获取一桌的所有订单
        vector<Order*> results;
        for (auto &c:orders) {
            if (c.gettablenumber()==tablenumber) {results.push_back(&c);}
        }
            return results;
    }
    map<int,int>getdishsalesranking() {//获取菜品销售排行
        map<int,int>results;
        for (auto &c:orders) {
            for (auto &d:c.getorderedfood()) {
                results[d.first]+=d.second.second;//将下单的菜品在results中累加份数
            }
        }
        return results;
    }


    //统计收入
    double calaulatetotalrevenue() {//统计总收入
        double total=0.0;
        for (auto &c:orders) {
            total+=c.calaulatetotal();
        }
        return total;
    }
    double calculattodayrevenue() {//统计当日总收入
        double today=0.0;
        auto time=chrono::system_clock::now();
        time_t now=chrono::system_clock::to_time_t(time);
        tm t1;
        localtime_s(&t1,&now);
        stringstream ss;
        ss<<put_time(&t1,"%Y:%m:%d");
        for (auto &c:orders) {
             if (c.gettime().find(ss.str())!=string::npos) {//查找订单时间是否存在今日日期字符串
                 today+=c.calaulatetotal();
             }
        }
        return today;
    }
        void displayallordersbytable(int tablenumber) {
            auto alorders=getordersbytable(tablenumber);
            if (alorders.empty()){cout<<tablenumber<<"的订单为0"<<endl;return;}
             cout<<tablenumber<<"的所有订单如下："<<endl;
            for (auto &c:alorders) {
                c->displayorder();
            }
        }

    void displaydishssalesranking() {
        auto ranking=getdishsalesranking();
        cout<<"\n======销量排行======"<<endl;
        if (ranking.empty()){cout<<"暂无销量数据"<<endl;return;}
        vector<pair<int,int>>rank(ranking.begin(),ranking.end());//转化为vector便于后面的排序
        sort(rank.begin(),rank.end(),[](const pair<int,int>&a,const pair<int,int>&b) {
         return a.second>=b.second;
        });
        int rankcount=0;
        for (auto &c:rank) {
            cout<<menumanager->getdish(c.first)->name<<"   销量*"<<c.second<<endl;
        }


    }
    void displayordersstatic() {//显示订单数据统计
        int totalcount=orders.size();
        double revenue1=calaulatetotalrevenue();
        double revenue2=calculattodayrevenue();
        cout << "\n=== 订单统计 ===" << endl;
        cout << "总订单数: " << totalcount << endl;
        cout << "总收入: ￥" << revenue1 << " 元" << endl;
        cout << "今日收入：￥"<<revenue2<<"元"<<endl;
        cout << "平均订单金额: ￥" << (totalcount > 0 ? revenue1 / totalcount : 0) << " 元" << endl;
    }

    //文件操作
    bool savetofile(string filename="orders.txt") {
        ofstream fout;
        fout.open(filename,ios::out);
        if (!fout.is_open()) {
            cout<<"文件"<<filename<<"打开失败"<<endl;
            return false;
        }
        fout<<nextid<<endl;

        for (auto &c:orders) {
            string statusstr;
            if (c.getstatus()==orderstatus::Pending) {statusstr="Pending";}
            else if (c.getstatus()==orderstatus::Preparing){statusstr="Preparing";}
            else if (c.getstatus()==orderstatus::Completed) {statusstr="Completed";}
            else statusstr="Cancelled";

            fout<<c.getorderid()<<","
                <<c.gettablenumber()<<","
                <<statusstr<<","
                <<c.gettime()<<endl;
            for (auto& d:c.getorderedfood()) {
                fout<<d.first<<":"<<d.second.second<<";";
            }
            fout<<endl;
        }
        fout.close();
        cout<<"订单数据已经保存到："<<filename<<endl;
        return true;
    }
    bool  loadfromfile(string filename="orders.txt") {
        ifstream fin;
        int loadcount=0;
        fin.open(filename,ios::in);
        if (!fin.is_open()) {
            cout<<"无法打开"<<filename<<endl;
            return false;
        }
        orders.clear();//清空订单集合

        //将文件第一行的数据变为id生成器
        string line;
        getline(fin,line);
        nextid=stoi(line);

        //读取订单号。桌号，和订单菜品信息
        int orderid,tablenumber;
        while (getline(fin,line)) {
            if (line.empty())continue;
            stringstream ss(line);
            string idstr,tablestr,timestr,statusstr;
            getline(ss,idstr,',');
            getline(ss,tablestr,',');
            getline(ss,timestr,',');
            getline(ss,timestr);

            //字符串的转化
            orderid=stoi(idstr);
            tablenumber=stoi(tablestr);
            orders.emplace_back(orderid,tablenumber);//在orders订单集里创建订单
            Order& od=orders.back();
            if (statusstr=="Pending"){od.setstatus(orderstatus::Pending);}
            else if (statusstr=="Preparing"){od.setstatus(orderstatus::Completed);}
            else if (statusstr=="completed"){od.setstatus(orderstatus::Canceled);}
            else od.setstatus(orderstatus::Canceled);
            //读取菜品数据
            if (getline(fin,line)){
                stringstream ss1(line);
                string dishstr;
                while(getline(ss1,dishstr,';')) {//每道菜品由;分割
                    if (dishstr.empty())continue;
                    size_t pos=dishstr.find(':');//利用string的find找到":"分割符号，‘：'分隔了菜品id和菜品份数
                    int dishid=stoi(dishstr.substr(0,pos));//子字符串为原字符串的位置（0-pos-1）的字符
                    int dishquantity=stoi(dishstr.substr(pos+1));
                    Dish* dish=menumanager->getdish(dishid);
                    if (dish) {
                        od.addfood(dish,dishquantity);
                    }
                }
            }
            loadcount++;
        }
        fin.close();
        cout<<"加载了"<<loadcount<<"订单"<<endl;
        return true;
    }
};

//顾客模式
class Customermode {
private:
    Menumanager* menumanager;
    Ordermanager* ordermanager;
    int currentorderid;
public:
    Customermode(Menumanager* m,Ordermanager* o,int currentorderid=-1) {//构造函数，需要先构造一个menumanger,ordermanager的实例对象然后传入
        menumanager=m;
        ordermanager=o;
        currentorderid=-1;//默认当前处理订单为-1，即为无待处理订单
    }

    //订单管理
    int createneworder(int tablenumber=0) {//创建新订单
        Order* newod=ordermanager->createorder(tablenumber);//创建了一个ordermanager中的orders(订单集中)中创建了新订单，返回了这个订单的指针
        if (newod)return newod->getorderid();//查询新订单编号然后返回
        return -1;//没创建成功就返回-1

    }
    bool setcurrentorder(int id) {//设置要处理修改的订单
       Order* od=ordermanager->getorder(id);
        if (od&&od->getstatus()==orderstatus::Pending) {
            currentorderid=id;//将当前处理的订单设置为id订单
            return true;
        }
        return false;//如果不存在，或是状态不为待处理则不可修改处理

    }
    int getorderid() {
        return currentorderid;//将现在处理的订单号返回
    }

    //菜品操作
    bool addfood_to_currentorder(int dishid,int quantity=1) {
        //增添多少份菜品到当天处理的订单中
        Dish* newds=menumanager->getdish(dishid);//动用menumanager的成员函数查找到与菜品编号相对应的菜
        Order* od=ordermanager->getorder(currentorderid);//动用ordermanager的成员函数查找到现在处理的订单
        if (!newds) {//菜品不存在
            cout<<"菜品"<<dishid<<"不存在"<<endl;
            return false;
        }
        if (!isrevisableorder()) {//当前订单号不可处理
            //ui设计后面需要让顾客知道是什么原因不可处理，通过getortderid来知道不可处理的原因，orderid=-1就是未初始化处理订单
            //不为-1，就是该订单处于不能处理阶段，一样需要重新设置处理订单号setcurrentorder;
            return false;
        }
        return ordermanager->addfoodtoorder(currentorderid,dishid,quantity);
    }
    bool removefood_from_currentorder(int dishid) {//删除订单特定食物
        Dish* ds=menumanager->getdish(dishid);
        if (!ds) {//菜品不存在
            cout<<"菜品"<<dishid<<"不存在"<<endl;
            return false;
        }
        if (!isrevisableorder()) {//当前订单号不可处理
            //ui设计后面需要让顾客知道是什么原因不可处理，通过getortderid来知道不可处理的原因，orderid=-1就是未初始化处理订单
            //不为-1，就是该订单处于不能处理阶段，一样需要重新设置处理订单号setcurrentorder;
            return false;
        }
        return ordermanager->removefoodfromorder(currentorderid,dishid);
    }
    bool updatefoodquantity(int dishid,int quantity) {//修改订单食物数量
        Dish* ds=menumanager->getdish(dishid);
        if (!ds) {//菜品不存在
            cout<<"菜品"<<dishid<<"不存在"<<endl;
            return false;
        }
        if (!isrevisableorder()) {//当前订单号不可处理
            //ui设计后面需要让顾客知道是什么原因不可处理，通过getortderid来知道不可处理的原因，orderid=-1就是未初始化处理订单
            //不为-1，就是该订单处于不能处理阶段，一样需要重新设置处理订单号setcurrentorder;
            return false;
        }
        ordermanager->removefoodfromorder(currentorderid,dishid);//先移除
        return ordermanager->addfoodtoorder(currentorderid,dishid,quantity);//再添加
    }

    //搜索功能
    vector<Dish*>searchrelateddishes(const string& prefix) {//查找相关食物
        return menumanager->searchdishes(prefix);//返回的是查找到的食物指针集，指针指向了原食物的位置
    }
    vector<Dish*>getdishesbycategory(const string& category) {//根据类名查找该类食物
        return menumanager->getdishbycategory(category);//返回的是根据类名查找到的该类食物指针集
    }
    vector<Dish*>getalldishes() {//获取所有菜品
        vector<Dish> aldishs=menumanager->getallfood();
        vector<Dish*>res;
        for (auto&c:aldishs) {
            res.push_back(&c);
        }
        return res;
    }

   //订单状态
    bool submitorder() {//提交订单（设置为准备中)
        if (!isrevisableorder())return false;
        ordermanager->getorder(currentorderid)->setstatus(orderstatus::Preparing);
        currentorderid=-1;//设置当前无处理订单
        return true;
    }
    bool cancelorder() {//取消订单
        if (!isrevisableorder())return false;
        ordermanager->getorder(currentorderid)->setstatus(orderstatus::Canceled);
        currentorderid=-1;//设置当前无处理订单
        return true;
    }
    bool getcurrentorderdetails() {//展示当前订单信息
        if (!isrevisableorder())return false;
        Order* od=ordermanager->getorder(currentorderid);
        od->displayorder();//直接输出显示订单细节
        return true;
    }
    Order* getcurrentorder() {//获取当前订单
        if (!isrevisableorder())return nullptr;
       return ordermanager->getorder(currentorderid);
    }
    double getcurrentordertotal() {//获取当前订单总价格
        if (!isrevisableorder())return 0;
        return ordermanager->getorder(currentorderid)->calaulatetotal();
    }
    //信息获取
    vector<string> getallcategories() {//获取所有食物种类
      return menumanager->getallcategory();
    }
    bool isorderactive() {//检查订单是否活跃，可能有用
        return currentorderid!=-1;
    }
    //清空当前订单(用于界面重置)
    void clearcurrentorder() {
        currentorderid=-1;
    }
    //获取当前桌号的所有订单（包括历史订单）
    vector<int>getallordersfortable(int tablenumber) {
        vector<int> res;
        if (!ordermanager->getorder(tablenumber)){return res;}
        vector<Order*>tableorders=ordermanager->getordersbytable(tablenumber);
        for (auto& c:tableorders) {
            res.push_back(c->getorderid());
        }
        return res;
    }
    //获取当前桌号所有可处理订单
    vector<int>getallrevisableorderfortable(int tablenumber) {
        vector<int> pendingordersid;
        if (!ordermanager->getorder(tablenumber)){return pendingordersid;}
        // 获取该桌号的所有订单
        vector<Order*> tableorders = ordermanager->getordersbytable(tablenumber);

        // 筛选状态为Pending的订单
        for (Order* order : tableorders) {
            if (order && order->getstatus() == orderstatus::Pending) {
                pendingordersid.push_back(order->getorderid());
            }
        }

        // 按订单ID排序（新的在前）
        std::sort(pendingordersid.begin(), pendingordersid.end(), std::greater<int>());

        return pendingordersid;
    }

private:
    bool isrevisableorder() {//检查当前订单号是否可以处理
        if (currentorderid==-1) return false;//无待处理订单返回false，不可修改处理
        Order* od=ordermanager->getorder(currentorderid);
        return od&&od->getstatus()==orderstatus::Pending;//判断订单是否存在且订单是否处于待处理状态，如果都是，则可以修改

}
};

//商家管理模式类
class Adminmode {
public:
    Menumanager* menumanager;
    Ordermanager* ordermanager;

public:
    Adminmode(Menumanager* menumanager,Ordermanager* ordermanager) {
        this->menumanager=menumanager;
        this->ordermanager=ordermanager;
    }
    ~Adminmode(){}

    //菜品管理
    bool adddish(string name,string category,double price ,int stock,string description) {//增添新菜品
        return menumanager->addfood(name,category,price,stock,description);
    }
    bool removedish(int dishid) {//通过菜品ID移除
        return menumanager->removefood(dishid);
    }
    bool removefood(string dishname) {//通过菜品名字移除
        return menumanager->removefood(dishname);
    }
    bool updatedish(int dishid,string newname,string newcategory,double newprice,int newstock,string newdescription) {//更新菜品信息
        return menumanager->updatefood(dishid,newname,newcategory,newprice,newstock,newdescription);
    }

    //菜品查询
    Dish* getdishbyid(int dishid) {//通过id查询菜品，返回菜品指针
        return menumanager->getdish(dishid);
    }
    Dish* getdishbyname(string name) {//通过菜名查询菜品
        return menumanager->getdish(name);
    }
    vector<Dish>getalldishes() {//获取所有食物集
        vector<Dish>alldishes;
        for (auto& c:menumanager->getallfood()) {
            alldishes.push_back(c);
        }
        return alldishes;
    }
    vector<Dish*>getalldishesbycategory(string category) {//获取一个类的相关菜品
        return menumanager->getdishbycategory(category);
    }
    vector<Dish*>searchdishes(string prefix) {//根据前缀查询相关菜品
        return menumanager->searchdishes(prefix);
    }

    //分类管理
    vector<string>getallgategories() {//获取所有种类
        return menumanager->getallcategory();
    }
    bool iscategoryexits(string category) {//查找类是否存在
        return menumanager->iskindexit(category);
    }
    int getdishcountincategory(string category) {//查询特定类的菜品总数
        return menumanager->getnumberinonekind(category);
    }

    //==订单管理办法==

    //订单查询
    vector<Order*>getallorders() {//获取所有订单集
        return ordermanager->getallorders();
    }
    vector<Order*>getordersbystatus(orderstatus status) {//获取特定状态的订单集
        return ordermanager->getordersbystatus(status);
    }
    vector<Order*>getordersbytable(int tablenumber) {//通过桌号查询订单
        return ordermanager->getordersbytable(tablenumber);
    }
    vector<Order*>getPendingorders() {//获取待处理订单
        return ordermanager->getpendingorders();
    }
    vector<Order*>getPreparingorders() {//获取正在准备的订单
        return ordermanager->getpreparingorders();
    }
    vector<Order*>getCompletedorders() {//获取已完成订单
        return ordermanager->getcompletedorders();
    }
    vector<Order*>getCanceledorders() {//获取被取消的订单
        return ordermanager->getcanceledorders();
    }
    Order* getorderbyid(int orderid) {//通过订单号获取特定订单
        return ordermanager->getorder(orderid);
    }
    //订单操作
    bool updateorderstatus(int orderid,orderstatus newstatus) {//更新订单状态
        Order* od=ordermanager->getorder(orderid);
        if (!od)return false;
        if (newstatus==orderstatus::Pending||newstatus==orderstatus::Preparing||newstatus==orderstatus::Completed||
            newstatus==orderstatus::Canceled) {
            od->setstatus(newstatus);
        }
        else {
            cout<<"菜单状态设置不成功，设置状态有误！"<<endl;
            return false;
        }

        if (od->getstatus()==newstatus)return true;
    }
    bool deleteorder(int orderid) {//删除订单
      return ordermanager->deleteorder(orderid);
    }

    //==统计分析方法
    //销售统计
    double gettotalrevenue() {//计算总收入
        return ordermanager->calaulatetotalrevenue();
    }
    double gettodayrevenue() {//计算今日收入
        return ordermanager->calculattodayrevenue();
    }
    int gettotalorderscount() {//获取所有订单数
        return ordermanager->getallorders().size();
    }
    int gettodayorderscoumt() {//获取当日订单数
        int today=0;
        auto time=chrono::system_clock::now();
        time_t now=chrono::system_clock::to_time_t(time);
        tm t1;
        localtime_s(&t1,&now);
        stringstream ss;
        ss<<put_time(&t1,"%Y:%m:%d");
        for (auto &c:(ordermanager->getallorders())) {
            if (c->gettime().find(ss.str())!=string::npos) {//查找订单时间是否存在今日日期字符串
                today++;
            }
        }
        return today;
    }

    //菜品统计
    map<int,int>getdishranking() {//获取订单排行map
        return ordermanager->getdishsalesranking();
    }
    void getsalesranking() {//展示销量排行
        ordermanager->displaydishssalesranking();
    }

    //菜单统计
   int gettotaldishcoount() {//获取菜单菜品总数
        return menumanager->getallfoodnum();
    }
    int getcategorycount() {//获取菜单的种类数
        return menumanager->getallcategorynum();
    }
    void getcategorywithnum(map<std::string, int>& stats) {//获取一个存储类名和类菜品数的map
        auto cates=menumanager->getallcategory();
        for (auto& c:cates) {
            stats[c]=menumanager->getnumberinonekind(c);
        }
    }

    //数据持久化
    bool savamenudate(string filename="food.txt") {
        return menumanager->savetofile(filename);
    }
    bool loadmenudate(string filename="food.txt") {
        return menumanager->loadfromfile(filename);
    }
    bool saveorderdate(string filename="orders.txt") {
        return ordermanager->savetofile(filename);
    }
    bool loadorderdate(string filename="orders.txt") {
        return ordermanager->loadfromfile(filename);
    }












};