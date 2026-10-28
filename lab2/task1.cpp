#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>

using namespace std;

queue<int> dataQueue;
mutex m;
bool flag = false;

bool IsPrime(int n) {
    if (n < 2) return false;

    for (int i = 2; i <= n / i; ++i) {
        if (n % i == 0) return false;
    }
    return true;
}

void DataPreparation() {
    int number;

    cout << "Enter integers (q to finish):\n";

    while (cin >> number) {
        lock_guard<mutex> lock(m);
        dataQueue.push(number);
    }

    {
        lock_guard<mutex> lock(m);
        flag = true;
    }
}

void DataProcessing() {
    unique_lock<mutex> lk(m);

    while (!flag) {
        lk.unlock();
        this_thread::sleep_for(chrono::milliseconds(100));
        lk.lock();
    }

    cout << "Prime numbers: ";

    while (!dataQueue.empty()) {
        int number = dataQueue.front();
        dataQueue.pop();

        if (IsPrime(number)) {
            cout << number << " ";
        }
    }

    cout << endl;
}

int main() {
    thread preparation(DataPreparation);
    preparation.detach();

    thread processing(DataProcessing);
    processing.join();

    return 0;
}
