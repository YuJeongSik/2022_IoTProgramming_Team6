#include<termios.h>
#include<stdio.h>
#include<stdlib.h>
#include <stdbool.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include <sys/ioctl.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include<asm/ioctls.h>
#include<time.h>

#define clcd "/dev/clcd"

static char tactswDev[] = "/dev/tactsw";
static int tactswFd = (-1);

unsigned char tactsw_get(int tmo)	//tact스위치 클릭값
{
	unsigned char b;
	if (tmo) {
		if (tmo < 0)
			tmo = ~tmo * 1000;
		else
			tmo *= 1000000;
		while (tmo > 0) {
			usleep(10000);
			read(tactswFd, &b, sizeof(b));
			if (b) return(b);
			tmo -= 10000;
		}
		return(-1);
	}
	else {
		read(tactswFd, &b, sizeof(b));
		return(b);
	}
}

int tact_switch_listener() {		// tact_switch값 반환
	unsigned char c;
	int selected_tact = 0;

	if ((tactswFd = open(tactswDev, O_RDONLY)) < 0) {
		perror("tact error");
		exit(-1);
	}

	while (1) {
		c = tactsw_get(12);		// tactswitch에 값 부여
		switch (c) {
		case 1: selected_tact = 1; break;
		case 2: selected_tact = 2; break;
		case 3: selected_tact = 3; break;
		case 4: selected_tact = 4; break;
		case 5: selected_tact = 5; break;
		case 6: selected_tact = 6; break;
		case 7: selected_tact = 7; break;
		case 8: selected_tact = 8; break;
		case 9: selected_tact = 9; break;
		case 10: selected_tact = 10; break;	// - 버튼 및 지우기 버튼
		case 11: selected_tact = 0; break;
		case 12: selected_tact = 11; break;	// 확인 버튼
		}
		return selected_tact;
	}
}

void clcd_input(char clcd_text[]) {			// clcd 글자 입력 및 출력
	int clcd_d;
	clcd_d = open(clcd, O_RDWR);
	if (clcd_d < 0) {
		printf("clcd error\n");
	}

	write(clcd_d, clcd_text, strlen(clcd_text));
	close(clcd_d);
}

int number;

int countLCD1(int number) {		// 숫자를 출력하는데 쓸 함수
	int clcd_d;
	char count[2];  //clcd에 출력할 숫자 char로 저장 
	char clcd_text1[30] = "max : ";  //clcd에 출력할 문자 저장 

	clcd_d = open(clcd, O_RDWR);
	if (clcd_d < 0) { printf("clcd error\n"); }

	sprintf(count, "%d", number); //정수형 number을 count에 저장 
	strcat(clcd_text1, count); //clcd_text와 count이어서 문자열로 저장 

	write(clcd_d, clcd_text1, strlen(clcd_text1)); //clcd에 최종 clcd_text출력
	close(clcd_d);
}

int countLCD2(int number) {		// 숫자를 출력하는데 쓸 함수
	int clcd_d;
	char max[1];
	char clcd_text2[30] = "how many num? : ";

	clcd_d = open(clcd, O_RDWR);
	if (clcd_d < 0) { printf("clcd error\n"); }

	sprintf(max, "%d", number);
	strcat(clcd_text2, max);

	write(clcd_d, clcd_text2, strlen(clcd_text2));
	close(clcd_d);
}

int countLCD3(int number) {		// 숫자를 출력하는데 쓸 함수
	int clcd_d;
	char num[10];
	char clcd_text3[10] = " ";

	clcd_d = open(clcd, O_RDWR);
	if (clcd_d < 0) { printf("clcd error\n"); }

	sprintf(num, "%d", number);
	strcat(clcd_text3, num);

	write(clcd_d, clcd_text3, strlen(clcd_text3));
	close(clcd_d);
}

int scoreLCD(int number) {		// 사용자의 점수를 확인하는데 쓰는 함수	
	int clcd_d;
	char score[10];
	char clcd_text4[30] = "score : ";

	clcd_d = open(clcd, O_RDWR);
	if (clcd_d < 0) { printf("clcd error\n"); }

	sprintf(score, "%d", number);
	strcat(clcd_text4, score);

	write(clcd_d, clcd_text4, strlen(clcd_text4));
	close(clcd_d);
}

int usransLCD(int number) {		// 사용자가 적은 답안을 보여주는데 쓰는 함수
	int clcd_d;
	char usrans[10];
	char clcd_text5[30] = "your answer : ";

	clcd_d = open(clcd, O_RDWR);
	if (clcd_d < 0) { printf("clcd error\n"); }

	sprintf(usrans, "%d", number);
	strcat(clcd_text5, usrans);

	write(clcd_d, clcd_text5, strlen(clcd_text5));
	close(clcd_d);
}

int main()
{
	int user_max_input = 0, count[2], trigger, i = 0, number;

	clcd_input("input max number");
	usleep(3000 * 1000);		// 3초의 대기시간
	clcd_input("maximum is 99");
	usleep(3000 * 1000);
	while (1) {
		count[user_max_input] = tact_switch_listener();  // 숫자 입력받기
		usleep(500000);

		if ((count[user_max_input] == 10 || count[user_max_input] == 11) && user_max_input == 0) {		// 숫자입력없이 바로 확인버튼을 눌렀을때 오류처리
			clcd_input("input number!!!");
			user_max_input = user_max_input - 1;
		}
		else if (count[user_max_input] == 11 && user_max_input == 1) {		// 확인 버튼 누르면 함수 탈출
			count[1] = count[0];	// 입력받은 숫자를 count[1]에 저장해서 아래서 사용
			number = count[1];		// 한자리의 숫자 출력
			countLCD1(number);
			usleep(2000 * 1000);
			break;
		}
		else if (count[user_max_input] == 11 && user_max_input == 2) {
			count[0] = count[0] * 10;		// 두자리 숫자 출력
			count[1] = count[0] + count[1];
			number = count[1];
			usleep(2000 * 1000);
			countLCD1(number);
			break;
		}
		user_max_input = user_max_input + 1;
	}


	clcd_input("choose level");
	usleep(3000 * 1000);
	clcd_input("maximum is 9");
	usleep(3000 * 1000);

	trigger = tact_switch_listener();		// 연산할 숫자 개수 입력

	while (1) {
		if (trigger == 0 || trigger == 10 || trigger == 11) {		// 1~9이외의 버튼을 눌렀을시 error표시
			clcd_input("put correct num!");
			trigger = tact_switch_listener();
		}
		else
			break;
	}

	number = trigger;

	countLCD2(number);			// 입력받은 숫자 보여주기
	usleep(3000 * 1000);

	clcd_input("inform input!");
	usleep(1000 * 3000);
	clcd_input("game start!");
	usleep(1000 * 3000);

	int lastscore = 0;
	int q;

	for (q = 0; q < 5; q++) {				// 게임을 5번 반복

		int data[trigger];
		int op2[trigger - 1];

		srand((int)time(NULL));						// 랜덤숫자 배열로 저장
		for (i = 0; i < trigger; i++) {
			data[i] = rand() % count[1] + 1;		//data[i]는 숫자 저장
		}

		for (i = 0; i < trigger - 1; i++) {	
			op2[i] = rand() % 2;			// op2는 부호 저장
		}

		i = 0;
		int ans = data[0];
		int clcd_d;
		int number;

		while (1) {										// 시스템상 정답 계산
			if (op2[i-1] == 1 && (i-1) >= 0) {						// 1 일때가 +
				ans = ans + data[i];
			}
			else if (op2[i-1] == 0 && (i-1) >= 0) {					// 0 일때가 -
				ans = ans - data[i];
			}

			if (i == (trigger - 1)) {
				break;
			}
			i += 1;
		}

		i = 0;

		while (1) {	// 숫자 및 부호 출력

			number = data[i];
			countLCD3(number);			// 임의의 숫자 출력
			usleep(2000 * 1000);

			if (op2[i] == 1) {			// 임의의 부호 출력
				if (i == (trigger-1)) {
					break;
				}
				clcd_input(" + ");
				usleep(2000 * 1000);
			}
			else {
				if (i == (trigger-1)) {
					break;
				}
				clcd_input(" - ");
				usleep(2000 * 1000);
			}
			i += 1;
		}

		clcd_input("input answer");
		usleep(3000 * 1000);

		int usr_ans = 0, mark=0;
		int count_input_num = 0, counting[4] = { 0 }; // counting[4] 배열크기가 4인 이유 99*9하면 최대값이 891이라 3자리가 최대

		usleep(500000);

		while (1) {
			counting[count_input_num] = tact_switch_listener();  // 숫자 입력받기
			usleep(500000);

			if (counting[0] == 11) {		// 숫자입력없이 바로 확인버튼을 눌렀을때 오류처리
				clcd_input("input number!!!");
				count_input_num = count_input_num - 1;
			}
			else if (counting[0] == 10 && mark == 0) {			// mark가 1일때 음수
				mark = 1;
				count_input_num = count_input_num - 1;
			}
			else if (counting[0] == 10 && mark == 1) {			// mark가 0일때 양수
				mark = 0;
				count_input_num = count_input_num - 1;
			}
			else if (count_input_num >= 1 && counting[count_input_num] == 10) {			// 지우기 버튼
				counting[count_input_num - 1] = 0;
				count_input_num = count_input_num - 2;
			}
			else if (counting[count_input_num] == 11 && count_input_num == 1) {		// 확인 버튼 누르면 함수 탈출
				counting[1] = counting[0];	// 입력받은 숫자를 counting[1]에 저장해서 아래서 사용
				number = counting[1];		// 한자리 숫자 출력
				usr_ans = number;
				if (mark == 1) {			// 확인 버튼을 입력받고 mark가 1일때 입력받은 값을 음수로 전환
					number = -number;
				}
				usransLCD(number);
				usleep(2000 * 1000);
				break;
			}
			else if (counting[count_input_num] == 11 && count_input_num == 2) {		// 두 자리 숫자 출력
				counting[0] = counting[0] * 10;
				counting[1] = counting[0] + counting[1];
				number = counting[1];
				usr_ans = number;
				if (mark == 1) {
					number = -number;
				}
				usransLCD(number);
				usleep(2000 * 1000);
				break;
			}
			else if (counting[count_input_num] == 11 && count_input_num == 3) {		// 세 자리 숫자 출력
				counting[0] = counting[0] * 100;
				counting[1] = counting[0] + (counting[1] * 10);
				counting[2] = counting[1] + counting[2];
				number = counting[2];
				usr_ans = number;
				if (mark == 1) {
					number = -number;
				}
				usransLCD(number);
				usleep(2000 * 1000);
				break;
			}

			count_input_num = count_input_num + 1;
		}

		if (mark == 1) {		// 기존에 입력받은 값을 음수로 변환
			usr_ans = -usr_ans;
		}

		if (ans == usr_ans) {	
			lastscore = lastscore + 1;
			clcd_input("correct!");
			usleep(2000 * 1000);
		}
		else {
			clcd_input("wrong answer");
			usleep(2000 * 1000);
		}
	}
	number = lastscore;
	scoreLCD(number);
	usleep(2000 * 1000);

	clcd_input("FINISH");
	return 0;

}
