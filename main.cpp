#include <iostream>
#include<string>
#include<memory>
#include<vector>
#include<map>
#include<algorithm>
#include<fstream>
#include<sstream>
#include<chrono>

using namespace std;


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
    string status;//订单状态
    chrono::system_clock::time_point createtime;//这是c++的现代时间库chrono定义了一个时间点变量，保存订单的创建时间
    map<int,pair<Dish*,int>>ordereddfood;//储存点的菜品号，菜品以及数量
public:
    Order(int id,int tablenum) {
        this->orderid=id;
        this->tablenumber=tablenum;
    }
    bool addfood(Dish* c,int number) {
            orderedfood
    }









};
















int main() {





}