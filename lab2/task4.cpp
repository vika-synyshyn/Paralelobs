#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

using namespace std;

queue<int> dataQueue;
mutex m;
condition_variable cv;
bool finished = false;

bool IsPrime(int n) {
    if (n < 2) return false;

    for (int i = 2; i <= n / i; i++) {
        if (n % i == 0) return false;
    }

    return true;
}

void DataPreparation() {
    queue<int> numbers;
    int number;

    cout << "Enter integers (q to finish):\n";

    while (cin >> number) {
        numbers.push(number);
    }

    {
        lock_guard<mutex> lock(m);
        dataQueue.swap(numbers);
        finished = true;
    }

    cv.notify_one();
}

void DataProcessing() {
    unique_lock<mutex> lock(m);

    cv.wait(lock, [] {
        return !dataQueue.empty() || finished;
        });

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
