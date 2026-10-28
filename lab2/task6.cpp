#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <future>
#include <vector>

using namespace std;

bool IsPrime(int n) {
    if (n < 2) return false;

    for (int i = 2; i <= n / i; i++) {
        if (n % i == 0) return false;
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

void Worker(deque<packaged_task<int(int)>>& tasks,
    deque<int>& numbers,
    mutex& m, condition_variable& cv, bool& finished) {

    while (true) {
        packaged_task<int(int)> task;
        int n;

        {
            unique_lock<mutex> lock(m);

            cv.wait(lock, [&] {
                return !tasks.empty() || finished;
                });

            if (tasks.empty() && finished)
                return;

            task = move(tasks.front());
            tasks.pop_front();

            n = numbers.front();
            numbers.pop_front();
        }

        task(n);
    }
}

int main() {
    deque<packaged_task<int(int)>> tasks;
    deque<int> numbers;

    vector<future<int>> results;
    vector<int> positions;

    mutex m;
    condition_variable cv;
    bool finished = false;

    thread worker([&] {
        Worker(tasks, numbers, m, cv, finished);
        });

    int n;

    cout << "Enter prime positions (q to finish):\n";

    while (cin >> n) {
        if (n < 1) {
            cout << "n must be positive\n";
            continue;
        }

        packaged_task<int(int)> task(FindPrime);

        results.push_back(task.get_future());
        positions.push_back(n);

        {
            lock_guard<mutex> lock(m);
            tasks.push_back(move(task));
            numbers.push_back(n);
        }

        cv.notify_one();
    }

    {
        lock_guard<mutex> lock(m);
        finished = true;
    }

    cv.notify_one();
    worker.join();

    for (size_t i = 0; i < results.size(); i++) {
        cout << positions[i] << "-th prime: "
            << results[i].get() << endl;
    }

    return 0;
}
