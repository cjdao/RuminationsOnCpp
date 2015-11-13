#include <iostream>
#include <cassert>

template<typename T>
class Seq ;
template<typename T>
class SeqItem {
    friend class Seq<T>;
    SeqItem(const T&d, SeqItem *n):use(1), data(d), next(n){
        if (next)  ++next->use; 
    };

    int use;
    SeqItem<T> *next;
    T data;
};

template<typename T>
class Seq {
public:
    Seq():item(0){};
    Seq(T &v, const Seq& s):item(new SeqItem<T>(v,s.item)){};
    Seq(const Seq& s):item(s.item){if (item) ++item->use;};

    Seq& operator=(const Seq& s) {// s 并没有被改写，只是s.item->use被改写了, 所以可以用const修饰！！！
        if (s.item) ++s.item->use; 
        destroy(item);
        item = s.item;
        return *this;
    }
    ~Seq() {
        destroy(item);
    };

    T hd() const {
        if (item) return item->data;
        else throw "hd of an empty Seq";
    };

    Seq tl() const{
        if (item) return Seq(item->next);
        else throw "tl of an empty Seq";
    };

    operator bool() { return item!=0; };

    Seq& operator++() {
        if (item) {
            SeqItem<T> *temp = item;
            item = item->next;
            if (--item->use==0) delete temp;
        }
        return *this;    
    }
    
    T operator*()const{
        return hd(); 
    }

private:
    void destroy(SeqItem<T> *i) {
        SeqItem<T> *next = i;
        while (i&&--i->use==0) {
            next = i->next;
            delete i;
            i = next; 
        }
    }
    Seq(SeqItem<T> *si):item(si){if (item) ++item->use;};

    SeqItem<T> *item;
};


int main()
{
    // 空Seq
    Seq<int> si;
    assert(!si); // si is empty

    // 创建一个Seq
    int i=0;
    for(i=0; i!=10; i++) {
         si = Seq<int>(i, si);
    }
    
    int head;
    i = 9;
    for (; si; si=si.tl()) {
        head=si.hd();
        assert(head==i);
        i--;
    }

    for(i=0; i!=10; i++) {
         si = Seq<int>(i, si);
    }
     
    Seq<int>* si1 = new Seq<int>(si);
    Seq<int> si2 = si1->tl();
    delete si1;

    i = 8;
    for (; si2; si2=si2.tl()) {
        head=si2.hd();
        assert(head==i);
        i--;
    }

    std::cout << " ---OK---."  << std::endl;   

    return 0;
}
