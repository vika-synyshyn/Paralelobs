#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std;

mutex m;
condition_variable cv;
condition_variable done;
int i = 0;
bool stop = false;
bool printed = false;

void Waits(int number) {
    unique_lock<mutex> lock(m);

    cout << "Thread " << number << " is waiting\n";

    cv.wait(lock, [] { return i == 1 || stop; });

    if (!stop && !printed) {
        cout << "Message from thread " << number << endl;
        printed = true;
        done.notify_one();
    }
}

void Notify() {
    this_thread::sleep_for(chrono::milliseconds(100));

    {
        lock_guard<mutex> lock(m);
        i = 1;
        cout << "Notify one thread\n";
    }

    cv.notify_one();
}

int main() {
    thread Thread1(Waits, 1);
    thread Thread2(Waits, 2);
    thread Thread3(Waits, 3);

    thread notify(Notify);

    notify.join();

    {
        unique_lock<mutex> lock(m);
        done.wait(lock, [] { return printed; });
        stop = true;
    }

    cv.notify_all();

    Thread1.join();
    Thread2.join();
    Thread3.join();

    return 0;
}
