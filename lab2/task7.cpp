#include <iostream>
#include <thread>
#include <future>
#include <mutex>
#include <chrono>
#include <cmath>
#include <functional>
#include <utility>
#include <climits>

using namespace std;

bool IsPrime(int number) {
    if (number < 2) return false;

    for (int i = 2; i <= number / i; i++) {
        if (number % i == 0) return false;
    }

    return true;
}

int FindPrime(int n) {
    int count = 0;
    int number = 1;

    while (count < n) {
        number++;

        if (IsPrime(number))
            count++;
    }

    return number;
}

void FirstThread(int n, promise<int> first,
    promise<bool> signal, promise<int> second) {

    first.set_value(FindPrime(n));

    signal.set_value(true);

    second.set_value(FindPrime(n * 10));
}

void SecondThread(int n, future<bool> signal, mutex& outputMutex) {
    if (signal.get()) {
        this_thread::sleep_for(chrono::seconds(2));

        lock_guard<mutex> lock(outputMutex);
        cout << "sqrt(" << n << ") = " << sqrt(n) << endl;
    }
}

int main() {
    int n;

    cout << "Enter n: ";
    cin >> n;

    if (n < 1 || n > INT_MAX / 10) {
        cout << "Invalid n!" << endl;
        return 0;
    }

    promise<int> firstPromise, secondPromise;
    promise<bool> startPromise;

    future<int> firstResult = firstPromise.get_future();
    future<int> secondResult = secondPromise.get_future();
    future<bool> startSignal = startPromise.get_future();

    mutex outputMutex;

    thread t1(FirstThread, n, move(firstPromise),
        move(startPromise), move(secondPromise));

    thread t2(SecondThread, n, move(startSignal), ref(outputMutex));

    int firstPrime = firstResult.get();

    {
        lock_guard<mutex> lock(outputMutex);
        cout << n << "-th prime: " << firstPrime << endl;
    }

    int secondPrime = secondResult.get();

    {
        lock_guard<mutex> lock(outputMutex);
        cout << n * 10 << "-th prime: " << secondPrime << endl;
    }

    t1.join();
    t2.join();

    return 0;
}
