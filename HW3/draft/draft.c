#include <iostream>
#include <string.h>
#include <iomanip>
#include <unistd.h>  //hostname
#include <sys/utsname.h> //uname
using namespace std;

int main()
{
	//start my code
	char hostname[100], new_name[256], sep[100] = "---------------";
	utsname uts_info;
	//string ss = "310551003-Virtualbox";
	//strcpy(new_name, ss.c_str());
	sethostname("310551003-Virtualbox", 20);
	gethostname(hostname, 100);
	
	//strcat(hostname, sep);
	sprintf(new_name,"new:\n%s\n%s",hostname,sep);
	cout << hostname << endl;
	cout << sep << endl;
	cout << new_name << endl;
	if (uname(&uts_info) < 0)
		perror("uname() error");
	else
		//printf("%*s%s\n", 10, "Kernel: ", uts_info.release);
		cout << left << setw(10) << "Kernel: " << uts_info.release << endl;  //set space
	return 0;
}
