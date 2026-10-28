#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std;

mutex m;
condition_variable cv;
int i = 0;

void Waits(int number) {
    unique_lock<mutex> lock(m);

    cout << "Thread " << number << " is waiting\n";

    cv.wait(lock, [] { return i == 1; });

    cout << "Thread " << number << " finished waiting\n";
}

void Awake() {
    this_thread::sleep_for(chrono::milliseconds(100));

    {
        lock_guard<mutex> lock(m);
        cout << "First notification\n";
    }
    cv.notify_all();

    this_thread::sleep_for(chrono::milliseconds(100));

    {
        lock_guard<mutex> lock(m);
        i = 1;
        cout << "Second notification\n";
    }
    cv.notify_all();
}

int main() {
    thread t1(Waits, 1);
    thread t2(Waits, 2);
    thread t3(Waits, 3);

    thread t4(Awake);

    t4.join();
    t1.join();
    t2.join();
    t3.join();

    return 0;
}
