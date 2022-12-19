#include <iostream>
#include <thread>

using namespace std;

void call_from_thread(int tid){
	cout << "thread On :  " << tid << endl;
	std::this_thread::sleep_for(std::chrono:: nanoseconds(tid*20));
}

int main(){
	thread t[10];

	thread(call_from_thread, 0).detach();
	for(int i = 1; i < 10; ++i){
		t[i] = thread(call_from_thread, i);
	}

	cout << " main function start " << endl;

	for(int i = 1; i < 10; ++i){
		if(t[i].joinable())
			t[i].join();
	}
	return 0;
}
