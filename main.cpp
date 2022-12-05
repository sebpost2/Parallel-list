#include <iostream>
#include <mutex>
#include <random>
#include <thread>

using namespace std;

template<class T>
struct Node
{
    Node(T x)
    {
        Nodo = nullptr;
        Valor = x;
        marca = 1;
    }
    Node<T>* Nodo;
    T Valor;
    bool marca;
};

template<class T>
struct Clist {
    Clist()
    {
        head = nullptr;
        tail = nullptr;
    }

    bool valide(Node<T>* pred, Node<T>* current);
    bool Add(T x);
    void exchange(Node<T>*& temp1, Node<T>*& temp2);
    bool Remove(T x);
    void print();

private:
    Node<T>* head;
    Node<T>* tail;
    mutex pred_lock;
    mutex current_lock;
    mutex next;
};
template<class T>
bool Clist<T>::valide(Node<T>* pred, Node<T>* current)
{
    if (!pred->marca || !current->marca)
    {
        return false;
    }
    return true;
}

template<class T>
bool Clist<T>::Add(T x)
{
    while (true) {
        Node<T>* s = new Node<T>(x);
        Node<T>* temp1 = head;
        Node <T>* temp2 = head;
        exchange(temp1, temp2);

        while (temp1 != nullptr && temp1->Valor < x)
        {
            temp2 = temp1;
            temp1 = temp1->Nodo;
        }
        pred_lock.lock();
        current_lock.lock();

        if (temp1 == nullptr)
        {
            head = s;
            pred_lock.unlock();
            current_lock.unlock();
            return true;
        }
        if (valide(temp2, temp1))
        {
            if (temp1->Valor == x)
            {
                pred_lock.unlock();
                current_lock.unlock();
                return false;
            }
            if (temp1 == head)
            {
                head = s;

            }
            else
            {
                temp2->Nodo = s;
            }
            s->Nodo = temp1;
            pred_lock.unlock();
            current_lock.unlock();
            return true;
        }

        pred_lock.unlock();
        current_lock.unlock();
    }

}

template<class T>
void Clist<T>::exchange(Node<T>* &temp1, Node<T>* &temp2) {
    Node<T>* extra = new Node<T>(-1);

    temp1 = head;
    extra->Nodo = temp1;
    temp2 = extra;
}

template<class T>
bool Clist<T>::Remove(T x)
{
    while (true) {
        Node<T>* s = new Node<T>(x);
        Node<T>* temp1 = head;
        Node <T>* temp2 = head;
        exchange(temp1, temp2);

        while (temp1 != nullptr && temp1->Valor < x)
        {
            temp2 = temp1;
            temp1 = temp1->Nodo;
        }
        pred_lock.lock();
        current_lock.lock();

        if (temp1 == nullptr)
        {
            pred_lock.unlock();
            current_lock.unlock();
            return false;
        }
        if (valide(temp2, temp1))
        {
            if (temp1->Valor != x)
            {
                pred_lock.unlock();
                current_lock.unlock();
                return false;
            }
            if (temp1 == head)
            {
                head = temp1->Nodo;
                temp2->Nodo = head;
                pred_lock.unlock();
                current_lock.unlock();
                return true;

            }
            else
            {
                temp2->Nodo = s;
            }

            temp2->Nodo = temp1->Nodo;
            delete temp1;

            pred_lock.unlock();
            current_lock.unlock();
            return true;
        }

        pred_lock.unlock();
        current_lock.unlock();
    }

}

template<class T>
void Clist<T>::print()
{
    Node<T>* temp1;
    temp1 = head;
    while (temp1 != nullptr)
    {
        cout << "->" << temp1->Valor;
        temp1 = temp1->Nodo;
    }
    cout << endl;
}


template<class T>
struct ADD
{

    Clist<T>* Clist_;
    int min, max;
    ADD(Clist<T>& list, int min, int max)
    {
        Clist_ = &list;
        this->min = min;
        this->max = max;
    }

    int get_random(int low, int high) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distribution(low, high);
        return distribution(gen);
    }

    void operator()(int operaciones) {
        for (int i = 0; i < operaciones; i++) {
            Clist_->Add(get_random(min, max));
        }
    }
};

template<class T>
struct REMOVE
{

    Clist<T>* Clist_;
    int min, max;
    REMOVE(Clist<T>& list, int min, int max)
    {
        Clist_ = &list;
        this->min = min;
        this->max = max;
    }

    int get_random(int low, int high) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distribution(low, high);
        return distribution(gen);
    }

    void operator()(int operaciones) {
        for (int i = 0; i < operaciones; i++) {
            Clist_->Remove(get_random(min, max));
        }
    }
};


int main()
{
    thread* threads[2];
    Clist<int> Clist;

    thread add_[8];
    thread remove_[8];
    for (int i = 0; i < 8; i++) {
        add_[i] = thread(ADD<int>(Clist,1,100000), 20);
        remove_[i] = thread(REMOVE<int>(Clist, 1, 100000), 20);
    }
    for (int i = 0; i < 8; i++) {
        add_[i].join();
        remove_[i].join();
    }

    Clist.print();
}