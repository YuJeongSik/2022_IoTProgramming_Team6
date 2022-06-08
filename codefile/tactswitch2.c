#include<termios.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
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

int main()
{
	int user_max_input = 0, count[2], trigger, i = 0;
	char save_num[2];

	clcd_input("input max number");
	usleep(5000 * 1000);		// 5초의 대기시간
	clcd_input("maximum is 99");
	usleep(5000 * 1000);
	while (1) {
		count[user_max_input] = tact_switch_listener();  // 숫자 입력받기

		if ((count[user_max_input] == 10 || count[user_max_input] == 11) && user_max_input == 0) {		// 숫자입력없이 바로 확인버튼을 눌렀을때 오류처리
			clcd_input("input number!!!");
			user_max_input -= 1;
		}
		else if (count[user_max_input] == 11 && user_max_input >= 0) {		// 확인 버튼 누르면 함수 탈출
			count[1] = count[0];	// 입력받은 숫자를 count[1]에 저장해서 아래서 사용
			break;
		}

		if (user_max_input == 1) {			// 숫자 두개(십의 자리)일때 실행
			count[0] = count[0] * 10;
			count[1] = count[0] + count[1];
			sprintf(save_num[0], "%d", count[1]);
			clcd_input("max number: %s", save_num[0]);		// 최대값 화면에 출력
			// 문자열로 바꿔서 해봤지만 똑같은 오류가 발생함
			// clcd_input <<< 이 함수는 한 개의 인자만을 받는데 두개의 인자를 받기에 발생하는 오류
			break;		// 숫자 두개를 입력받았으니 반복문 탈출
		}
		user_max_input += 1;
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

	clcd_input("최대값은 %d, 암산갯수는 %d입니다", count[1], trigger);		// 위에서 입력한 정보들 출력

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

}