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

unsigned char tactsw_get(int tmo)	//tach스위치 클릭값
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
		c = tactsw_get(10);		// 버튼 인식 안될시 숫자 바꿔보기
		switch (c) {
		case 1: selected_tact = 1;
		case 2: selected_tact = 2;
		case 3: selected_tact = 3;
		case 4: selected_tact = 4;
		case 5: selected_tact = 5;
		case 6: selected_tact = 6;
		case 7: selected_tact = 7;
		case 8: selected_tact = 8;
		case 9: selected_tact = 9;
		case 10: selected_tact = 10;	// - 버튼으로 사용 예정
		case 11: selected_tact = 0;
		case 12: selected_tact = 11;	// 일단은 확인버튼으로 사용
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

int countLCD1(int number) {
	int clcd_d;
	char count[2];
	char clcd_text1[30] = "최댓값 : ";

	clcd_d = open(clcd, O_RDWR);
	if (clcd_d < 0) { printf("clcd error\n"); }

	sprintf(count, "%d", number);
	strcat(clcd_text1, count);

	write(clcd_d, clcd_text1, strlen(clcd_text1));
	close(clcd_d);
}

int countLCD2(int number) {
	int clcd_d;
	char max[1];
	char clcd_text2[30] = "숫자개수 : ";

	clcd_d = open(clcd, O_RDWR);
	if (clcd_d < 0) { printf("clcd error\n"); }

	sprintf(max, "%d", number);
	strcat(clcd_text2, max);

	write(clcd_d, clcd_text2, strlen(clcd_text2));
	close(clcd_d);
}

int calculate(int num[], char op, int end) {
	int ans = 0, i;

	if (op == '+') {
		for (i = 0; i < end; i++) {
			ans += num[i];
		}
		return ans;
	}
	else {
		ans = num[0];
		for (i = 1; i < end; i++) {
			ans -= num[i];
		}
		return ans;
	}
}

int main()
{
	int user_max_input = 0, count[2], trigger, i = 0;

	clcd_input("input max number");
	usleep(5000 * 1000);		// 5초의 대기시간
	clcd_input("maximum is 99");
	usleep(5000 * 1000);
	while (1) {
		count[user_max_input] = tact_switch_listener();  // 숫자 입력받기

		if ((count[user_max_input] == 10 || count[user_max_input] == 11) && user_max_input == 0) {		// 숫자입력없이 바로 확인버튼을 눌렀을때 오류처리
			// user_max_input은 사용자가 숫자를 입력한 개수를 나타냄
			clcd_input("input number!!!");
			user_max_input -= 1;
		}
		else if (count[user_max_input] == 11 && user_max_input >= 0) {		// 확인 버튼 누르면 함수 탈출
			// 첫번째 숫자를 입력하고 while문의 두번째 사이클을 진행할때 확인 버튼이 눌리면 함수 탈출
			count[1] = count[0];	// 입력받은 숫자를 count[1]에 저장해서 아래서 사용
			break;
		}

		if (user_max_input == 1) {			// 숫자 두개(십의 자리)일때 실행
			count[0] = count[0] * 10;
			count[1] = count[0] + count[1];
			break;		// 숫자 두개를 입력받았으니 반복문 탈출
		}
		user_max_input += 1;

		int number = count[1];

		countLCD1(number);
		return 0;
	}


	clcd_input("what level?");
	usleep(5000 * 1000);
	clcd_input("maximum is 9");
	usleep(5000 * 1000);

	trigger = tact_switch_listener();

	while (1) {
		if (trigger == 0 || trigger == 10 || trigger == 11) {		// 1~9이외의 버튼을 눌렀을시 error표시
			clcd_input("error! max is 9");
			trigger = tact_switch_listener();
		}
		else
			break;
	}

	int number = trigger;

	countLCD2(number);
	return 0;


	clcd_input("최댓값과 숫자개수 선택완료");

	// 위에서 입력한 정보들 출력

	// 아래에 랜덤함수 기능 들어가야함

	int data[trigger];

	srand((int)time(NULL));
	for (i = 0; i < trigger; i++) {
		data[i] = rand() % count[1] + 1;
	}

	// 아래부분은 숫자 보여주는 부분

	i = 0;
	while (1) {
		clcd_input("%d", data[i]);
		usleep(2000 * 1000);
		if (i == trigger) {
			break;
		}
		i += 1;
	}


	// 랜덤으로 연산자 뽑기
	char op[2] = { '+', '-' };	// 사용할 연산자 종류
	char op2[10];	// 각 문제에 지정할 연산자
	int k;

	srand((int)time(NULL));
	for (int i = 0; i < 10; i++) {
		k = rand() % 2;
		op2[i] = op[k];
	}


	// 각각 지정된 연산자에 대한 정답계산
	int ans;

	for (int i = 0; i < 10; i++) {
		ans = calculate(data, op2[i], trigger);
	}

	// 정답 입력받기
	clcd_input("input answer");
	
	int usr_ans;
	int count_input_num = 0, counting[4]; // counting[4] 배열크기가 4인 이유 99*9하면 최대값이 891이라 3자리가 최대

	while (1) {

		counting[count_input_num] = tact_switch_listener();  // 음수(or 지우기)인지 양수인지 입력받기

		if (tact_switch_listener == 10 && count_input_num == 0) {			// 음수일때 실행, count_input_num은 현재 배열에 들어가있는 숫자의 수
			while (1) {
				counting[count_input_num] = tact_switch_listener();  // 숫자 입력받기, count_input_num 처음 while문 들어올때 0이어서 그대로 사용 가능

				if (counting[count_input_num] == 11 && count_input_num == 0) {		// 숫자입력없이 바로 확인버튼을 눌렀을때 오류처리
					clcd_input("input number!!!");
					count_input_num -= 1;
				}
				else if (counting[0] == 10 && count_input_num == 0) {		// 음수를 선택한 상태에서 음수를 한번 더 선택하면 다시한번 부호 선택
					break;
				}
				else if (counting[count_input_num] == 11 && count_input_num == 1) {		// 확인 버튼 누르면 함수 탈출
					counting[0] = counting[0] * (-1);	// 반복문 탈출할때 음수처리
					usr_ans = counting[0];
					break;
				}
				else if ((counting[count_input_num] == 11 && count_input_num == 2)) {		// 숫자 두개(십의 자리)일때 실행
					counting[0] = counting[0] * 10;
					counting[1] = counting[0] + counting[1];
					counting[1] = counting[1] * (-1);
					usr_ans = counting[1];
					break;		// 숫자 두개를 입력받았으니 반복문 탈출
				}
				else if ((counting[count_input_num] == 11 && count_input_num == 3)) {		// 숫자 세개(백의 자리)일때 실행
					counting[0] = counting[0] * 100;
					counting[1] = counting[1] * 10;
					counting[2] = counting[0] + counting[1] + counting[2];
					counting[2] = counting[2] * (-1);
					usr_ans = counting[2];
					break;		// 숫자 세개를 입력받았으니 반복문 탈출
				}
				else if (counting[count_input_num] == 10 && count_input_num >= 1) {		// 음수(지우기) 버튼을 눌렀을때 실행
					counting[count_input_num - 1] = 0;		// 이전 사이클에서 입력받은 숫자를 0으로 초기화하고
					count_input_num = count_input_num - 2;		// 배열 위치를 한단계 전으로 바꿈
				}
				count_input_num += 1;
			}
		}
		else {				// 정답이 양수일때 실행
			if (count[count_input_num] == 11 && count_input_num == 0) {		// 숫자입력없이 바로 확인버튼을 눌렀을때 오류처리
				clcd_input("input number!!!");
				count_input_num -= 1;
			}
			else if (counting[count_input_num] == 11 && count_input_num == 1) {		// 확인 버튼 누르면 함수 탈출
				// 한자리 숫자 출력
				usr_ans = counting[0];
				break;
			}
			else if ((counting[count_input_num] == 11 && count_input_num == 2)) {		// 숫자 두개(십의 자리)일때 실행
				counting[0] = counting[0] * 10;
				counting[1] = counting[0] + counting[1];
				usr_ans = counting[1];
				break;		// 숫자 두개를 입력받았으니 반복문 탈출
			}
			else if ((counting[count_input_num] == 11 && count_input_num == 3)) {		// 숫자 세개(백의 자리)일때 실행
				counting[0] = counting[0] * 100;
				counting[1] = counting[1] * 10;
				counting[2] = counting[0] + counting[1] + counting[2];
				usr_ans = counting[2];
				break;		// 숫자 세개를 입력받았으니 반복문 탈출
			}
			else if (counting[count_input_num] == 10 && count_input_num >= 1) {		// 음수(지우기) 버튼을 눌렀을때 실행
				counting[count_input_num - 1] = 0;		// 이전 사이클에서 입력받은 숫자를 0으로 초기화하고
				count_input_num = count_input_num - 2;		// 배열 위치를 한단계 전으로 바꿈
			}
			count_input_num += 1;
		}

		// usr_ans에 사용자 정답 저장
	}
}