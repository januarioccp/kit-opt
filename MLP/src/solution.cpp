#include "solution.h"
#include "colormod.h"
#include <cmath>
#include <climits>
using namespace std;

Solution::Solution(Input *input)
{

    this->in = input;
    contador = 0;
    // Create a generic basic solution
    for (unsigned i = 1; i <= this->in->dimensionGet(); i++)
        location.push_back(i);
    location.push_back(1);

    size = in->dimensionGet() + 1;
    duration = new int *[size];
    for (int i = 0; i < size; i++)
    {
        duration[i] = new int[size];
        duration[i][i] = 0;
    }

    cost = new int *[size];
    for (int i = 0; i < size; i++)
    {
        cost[i] = new int[size];
        cost[i][i] = 0;
    }
}

Solution::~Solution()
{
    for (int i = 0; i < size; i++)
    {
        delete[] duration[i];
        delete[] cost[i];
    }
    delete[] duration;
    delete[] cost;
}

double Solution::t_(unsigned i, unsigned j)
{
    return in->distanceGet(location[i], location[j]);
}

void Solution::reset()
{
    costValueMLP = INT_MAX;

    for (int i = 0; i < size; i++)
        for (int j = i; j < size; j++)
            if (i != j)
                duration[i][j] = -1;
            else
                duration[i][j] = 0;

    for (int i = 0; i < size; i++)
        for (int j = i; j < size; j++)
            if (i != j)
                cost[i][j] = -1;
            else
                cost[i][j] = 0;

    location.resize(0);
}

int Solution::W(int begin, int end)
{
    if (begin > end)
        swap(begin, end);
    if (begin == 0)
        begin++;
    if (end == 0)
        return 0;
    return end - begin + 1;
}

int Solution::T_recursive(int begin, int end)
{
    return duration[begin][end];
}

int Solution::C(int begin, int end)
{
    return cost[begin][end];
}

void Solution::computeCostValueMLP()
{

    for (int i = 0; i < size; i++)
        for (int j = i; j < size; j++)
            if (i != j)
                duration[i][j] = duration[i][j - 1] + in->distanceGet(location[j - 1], location[j]);
            else
                duration[j][j] = 0;

    for (int i = 0; i < size; i++)
        for (int j = i; j >= 0; j--)
            if (i != j)
                duration[i][j] = duration[i][j + 1] + in->distanceGet(location[j], location[j + 1]);
            else
                duration[j][j] = 0;

    for (int i = 0; i < size; i++)
        for (int j = i; j < size; j++)
            if (i != j)
                cost[i][j] = cost[i][j - 1] + duration[i][j];
            else
                cost[i][j] = 0;

    for (int i = 0; i < size; i++)
        for (int j = i; j >= 0; j--)
            if (i != j)
                cost[i][j] = cost[i][j + 1] + duration[i][j];
            else
                cost[i][j] = 0;

    costValueMLP = C(0, this->in->dimensionGet());
}

void Solution::updateSwap(int a, int b)
{
    if (a > b)
        swap(a, b);

    for (int i = 0; i <= a; i++)
        for (int j = a; j < size; j++)
            if (i != j)
                duration[i][j] = duration[i][j - 1] + in->distanceGet(location[j - 1], location[j]);
            else
                duration[j][j] = 0;

    for (int i = a + 1; i <= b; i++)
        for (int j = b; j < size; j++)
            if (i != j)
                duration[i][j] = duration[i][j - 1] + in->distanceGet(location[j - 1], location[j]);
            else
                duration[j][j] = 0;

    for (int i = a; i <= b; i++)
        for (int j = a; j >= 0; j--)
            if (i != j)
                duration[i][j] = duration[i][j + 1] + in->distanceGet(location[j], location[j + 1]);
            else
                duration[j][j] = 0;

    for (int i = b; i < size; i++)
    {
        for (int j = b; j >= 0; j--)
        {
            if (i != j)
                duration[i][j] = duration[i][j + 1] + in->distanceGet(location[j], location[j + 1]);
            else
                duration[j][j] = 0;
        }
    }

    for (int i = 0; i <= a; i++)
        for (int j = a; j < size; j++)
            if (i != j)
                cost[i][j] = cost[i][j - 1] + duration[i][j];
            else
                cost[i][j] = 0;

    for (int i = a + 1; i <= b; i++)
        for (int j = b; j < size; j++)
            if (i != j)
                cost[i][j] = cost[i][j - 1] + duration[i][j];
            else
                cost[i][j] = 0;

    for (int i = a; i <= b; i++)
        for (int j = a; j >= 0; j--)
            if (i != j)
                cost[i][j] = cost[i][j + 1] + duration[i][j];
            else
                cost[i][j] = 0;

    for (int i = b; i < size; i++)
        for (int j = b; j >= 0; j--)
            if (i != j)
                cost[i][j] = cost[i][j + 1] + duration[i][j];
            else
                cost[i][j] = 0;

    costValueMLP = C(0, this->in->dimensionGet());
}

void Solution::update2opt(int a, int b)
{
    if (a > b)
        swap(a, b);

    for (int i = 0; i <= b; i++)
        for (int j = max(a, i); j < size; j++)
            if (i != j)
                duration[i][j] = duration[i][j - 1] + in->distanceGet(location[j - 1], location[j]);
            else
                duration[j][j] = 0;

    for (int i = a; i < size; i++)
        for (int j = min(b, i); j >= 0; j--)
            if (i != j)
                duration[i][j] = duration[i][j + 1] + in->distanceGet(location[j], location[j + 1]);
            else
                duration[j][j] = 0;

    for (int i = 0; i <= b; i++)
        for (int j = max(a, i); j < size; j++)
            if (i != j)
                cost[i][j] = cost[i][j - 1] + duration[i][j];
            else
                cost[i][j] = 0;

    for (int i = a; i < size; i++)
        for (int j = min(b, i); j >= 0; j--)
            if (i != j)
                cost[i][j] = cost[i][j + 1] + duration[i][j];
            else
                cost[i][j] = 0;

    //this->printDuration();

    costValueMLP = C(0, this->in->dimensionGet());
}

void Solution::copy(const Solution *s)
{
    this->location = s->location;
    this->costValueMLP = s->costValueMLP;
}

void Solution::printCost()
{
    cout << endl;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            cout << setw(6) << cost[i][j];
        }
        cout << endl;
    }
}

void Solution::printDuration()
{
    cout << endl;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            cout << setw(5) << duration[i][j];
        }
        cout << endl;
    }
}

ostream &operator<<(ostream &out, const Solution &s)
{
    Color::Modifier red(Color::FG_RED);
    Color::Modifier deff(Color::FG_DEFAULT);

    if (s.location.size() <= s.in->dimensionGet())
        out << red << "Warning! Incomplete solution!!!" << deff << endl;

    out << "Dimension: " << s.in->dimensionGet() << endl;
    out << "Latency: " << setprecision(1) << fixed << s.costValueMLP << endl;
    out << "i = [";
    for (unsigned i = 0; i < s.location.size(); i++)
        out << setw(3) << i << " ";
    out << "]" << endl;
    out << "s = {";
    for (unsigned i = 0; i < s.location.size(); i++)
        if (i < s.location.size() - 1)
        {
            if (s.location[i] <= 7)
                out << "\033[" << 30 + s.location[i] << "m";
            if (8 <= s.location[i] && s.location[i] <= 14)
                out << "\033[" << 83 + s.location[i] << "m";
            out << setw(3) << s.location[i] << deff << ",";
        }
        else
        {
            if (s.location[i] <= 7)
                out << "\033[" << 30 + s.location[i] << "m";
            if (8 <= s.location[i] && s.location[i] <= 14)
                out << "\033[" << 83 + s.location[i] << "m";
            out << setw(3) << s.location[i] << deff;
        }
    out << "}" << endl;

    return out;
}