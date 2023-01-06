#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;
class regression {
    // 현재 진행 중인 동적 어레이
    // (i번째 x) (i번째 y)를 포함
    vector<float> x;
    vector<float> y;

    // 계수/기울기 저장 가장 적합한 선
    float coeff;

    // 상수 항 저장 위치 가장 적합한 선
    float constTerm;

    // 다음의 곱의 합을 포함합니다 (i번째 x) 및 (i번째 y)
    float sum_xy;

    // 모두의 합 포함(i번째 x), (i번째 y)
    float sum_x;
    float sum_y;

    // 제곱합 포함 (i번째 x), (i번째 y)의 합
    float sum_x_square;
    float sum_y_square;

public:
    // 기본값을 제공할 생성자의 모든 항에 대한 값
    // 계급 회귀의 대상
    regression()
    {
        coeff = 0;
        constTerm = 0;
        sum_y = 0;
        sum_y_square = 0;
        sum_x_square = 0;
        sum_x = 0;
        sum_xy = 0;
    }

    // 계수를 계산하는 함수 가장 적합한 선의 경사
    void calculateCoefficient()
    {
        float N = x.size();
        float numerator
            = (N * sum_xy - sum_x * sum_y);
        float denominator
            = (N * sum_x_square - sum_x * sum_x);
        coeff = numerator / denominator;
    }

    // 계산할 멤버 함수 // 최고의 불변 기간 적합선
    void calculateConstantTerm()
    {
        float N = x.size();
        float numerator
            = (sum_y * sum_x_square - sum_x * sum_xy);
        float denominator
            = (N * sum_x_square - sum_x * sum_x);
        constTerm = numerator / denominator;
    }

    // 계수를 반환하는 함수 가장 적합한 선의 경사
    float coefficient()
    {
        if (coeff == 0)
            calculateCoefficient();
        return coeff;
    }

    // 상수를 반환하는 함수 가장 적합한 선의 항
    float constant()
    {
        if (constTerm == 0)
            calculateConstantTerm();
        return constTerm;
    }

    // 가장 적합한 선 그리기
    void PrintBestFittingLine()
    {
        if (coeff == 0 && constTerm == 0) {
            calculateCoefficient();
            calculateConstantTerm();
        }
        cout << "The best fitting line is y = "
            << coeff << "x + " << constTerm << endl;
    }

    // 데이터 집합에서 입력을 가져오는 함수
    void takeInput(int n)
    {
        for (int i = 0; i < n; i++) {
            // 모든 값 xi와 yi는 쉼표 구분
            char comma;
            float xi;
            float yi;
            cin >> xi >> comma >> yi;
            sum_xy += xi * yi;
            sum_x += xi;
            sum_y += yi;
            sum_x_square += xi * xi;
            sum_y_square += yi * yi;
            x.push_back(xi);
            y.push_back(yi);
        }
    }

    // 데이터 세트를 표시하는 기능
    void showData()
    {
        for (int i = 0; i < 62; i++) {
            printf("_");
        }
        printf("\n\n");
        printf("|%15s%5s %15s%5s%20s\n",
            "X", "", "Y", "", "|");

        for (int i = 0; i < x.size(); i++) {
            printf("|%20f %20f%20s\n",
                x[i], y[i], "|");
        }

        for (int i = 0; i < 62; i++) {
            printf("_");
        }
        printf("\n");
    }


};

int main()
{
    regression reg;

    // 데이터 세트에서 (xi, yi)의 쌍 수
    int n = 5;

    // n쌍을 입력, 호출 함수 takeInput to 
    reg.takeInput(n);

    // 가장 적합한 선 그리기
    reg.PrintBestFittingLine();

    float coef, constant;
    coef = reg.coefficient();
    constant = reg.constant();

    printf("\n\n%fx + %f\n", coef, constant);
}
