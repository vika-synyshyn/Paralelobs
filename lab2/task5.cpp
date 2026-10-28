#include <iostream>
#include <future>
#include <cmath>
#include <chrono>

using namespace std;

bool IsPrime(int n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;

    for (int i = 3; i <= n / i; i += 2) {
        if (n % i == 0) return false;
    }

    return true;
}

int FindPrime(int n) {
    int count = 0;
    int number = 1;

    while (count < n) {
        number++;

        if (IsPrime(number)) {
            count++;
        }
    }

    return number;
}

int main() {
    int n, mode, choice;

    cout << "Enter prime number position: ";
    cin >> n;

    if (n < 1 || n > 1000000) {
        cout << "Invalid number!\n";
        return 0;
    }

    cout << "Choose mode:\n";
    cout << "1 - deferred\n";
    cout << "2 - async\n";
    cin >> mode;

    future<int> result;

    if (mode == 1) {
        result = async(launch::deferred, FindPrime, n);
    }
    else if (mode == 2) {
        result = async(launch::async, FindPrime, n);
    }
    else {
        cout << "Invalid mode!\n";
        return 0;
    }

    cout << "Choose function:\n";
    cout << "1 - sqrt\n";
    cout << "2 - sin\n";
    cout << "3 - log\n";
    cin >> choice;

    if (choice == 1) {
        cout << "sqrt = " << sqrt(n) << endl;
    }
    else if (choice == 2) {
        cout << "sin = " << sin(n) << endl;
    }
    else if (choice == 3) {
        cout << "log = " << log(n) << endl;
    }
    else {
        cout << "Invalid function!\n";
    }

    auto start = chrono::steady_clock::now();

    int prime = result.get();

    auto end = chrono::steady_clock::now();

    chrono::duration<double> elapsed = end - start;

    cout << "Prime number: " << prime << endl;
    cout << "Waiting time: " << elapsed.count()
        << " seconds\n";

    return 0;
}
