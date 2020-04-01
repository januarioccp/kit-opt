#include "solution.h"
#include "colormod.h"
#include <cmath>
#include <climits>
using namespace std;

Solution::Solution()
{
    size = 0;
}

Solution::Solution(Input *input)
{

    this->in = input;
    dimension = input->dimensionGet();
    contador = 0;
    // Create a generic basic solution
    for (int i = 1; i <= dimension; i++)
        location.push_back(i);
    location.push_back(1);

    size = dimension + 1;
    reOpt = new int **[2];
    reOpt[0] = new int *[size];
    reOpt[1] = new int *[size];

    for (int i = 0; i < size; i++)
    {
        reOpt[0][i] = new int[size];
        reOpt[1][i] = new int[size];
        reOpt[0][i][i] = 0;
        reOpt[1][i][i] = 0;
    }
}

Solution::~Solution()
{
    // for (int i = 0; i < size; i++)
    // {
    //     delete reOpt[0][i];
    //     delete reOpt[1][i];
    // }
    // delete duration;
    // delete cost;
}

void Solution::deleteMe()
{
    for (int i = 0; i < size; i++)
    {
        delete[] reOpt[0][i];
        delete[] reOpt[1][1];
    }
    delete[] reOpt[0];
    delete[] reOpt[1];

    delete[] reOpt;
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
                reOpt[0][i][j] = -1;
            else
                reOpt[0][i][j] = 0;

    for (int i = 0; i < size; i++)
        for (int j = i; j < size; j++)
            if (i != j)
                reOpt[1][i][j] = -1;
            else
                reOpt[1][i][j] = 0;

    location.resize(0);
}

int Solution::W(int begin, int end)
{
    if (begin > end)
    {
        if (!begin)
        {
            exit(0);
            return 0;
        }
        if (end == 0)
        {
            // exit(0);
            return begin - end + 2;
        }
        return begin - end + 1;
    }
    else
    {
        if (!end)
        {
            exit(0);
            return 0;
        }
        if (begin == 0)
        {
            exit(0);
            return end - begin + 2;
        }
        return end - begin + 1;
    }
}

int Solution::T_recursive(int begin, int end)
{
    return reOpt[0][begin][end];
}

int Solution::C(int begin, int end)
{
    return reOpt[1][begin][end];
}

void Solution::computeCostValueMLP()
{

    for (int i = 0; i < size; i++)
        for (int j = i; j < size; j++)
            if (i != j)
                reOpt[0][i][j] = reOpt[0][i][j - 1] + in->distanceGet(location[j - 1], location[j]);
            else
                reOpt[0][j][j] = 0;

    for (int i = 0; i < size; i++)
        for (int j = i; j >= 0; j--)
            if (i != j)
                reOpt[0][i][j] = reOpt[0][i][j + 1] + in->distanceGet(location[j], location[j + 1]);
            else
                reOpt[0][j][j] = 0;

    for (int i = 0; i < size; i++)
        for (int j = i; j < size; j++)
            if (i != j)
                reOpt[1][i][j] = reOpt[1][i][j - 1] + reOpt[0][i][j];
            else
                reOpt[1][i][j] = 0;

    for (int i = 0; i < size; i++)
        for (int j = i; j >= 0; j--)
            if (i != j)
                reOpt[1][i][j] = reOpt[1][i][j + 1] + reOpt[0][i][j];
            else
                reOpt[1][i][j] = 0;

    costValueMLP = C(0, this->dimension);
}

void Solution::updateSwap(int a, int b)
{
    if (a > b)
        swap(a, b);

    for (int i = 0; i <= a; i++)
        for (int j = a; j < size; j++)
            if (i != j)
                reOpt[0][i][j] = reOpt[0][i][j - 1] + in->distanceGet(location[j - 1], location[j]);
            else
                reOpt[0][j][j] = 0;

    for (int i = a + 1; i <= b; i++)
        for (int j = b; j < size; j++)
            if (i != j)
                reOpt[0][i][j] = reOpt[0][i][j - 1] + in->distanceGet(location[j - 1], location[j]);
            else
                reOpt[0][j][j] = 0;

    for (int i = a; i <= b; i++)
        for (int j = a; j >= 0; j--)
            if (i != j)
                reOpt[0][i][j] = reOpt[0][i][j + 1] + in->distanceGet(location[j], location[j + 1]);
            else
                reOpt[0][j][j] = 0;

    for (int i = b; i < size; i++)
    {
        for (int j = b; j >= 0; j--)
        {
            if (i != j)
                reOpt[0][i][j] = reOpt[0][i][j + 1] + in->distanceGet(location[j], location[j + 1]);
            else
                reOpt[0][j][j] = 0;
        }
    }

    for (int i = 0; i <= a; i++)
        for (int j = a; j < size; j++)
            if (i != j)
                reOpt[1][i][j] = reOpt[1][i][j - 1] + reOpt[0][i][j];
            else
                reOpt[1][i][j] = 0;

    for (int i = a + 1; i <= b; i++)
        for (int j = b; j < size; j++)
            if (i != j)
                reOpt[1][i][j] = reOpt[1][i][j - 1] + reOpt[0][i][j];
            else
                reOpt[1][i][j] = 0;

    for (int i = a; i <= b; i++)
        for (int j = a; j >= 0; j--)
            if (i != j)
                reOpt[1][i][j] = reOpt[1][i][j + 1] + reOpt[0][i][j];
            else
                reOpt[1][i][j] = 0;

    for (int i = b; i < size; i++)
        for (int j = b; j >= 0; j--)
            if (i != j)
                reOpt[1][i][j] = reOpt[1][i][j + 1] + reOpt[0][i][j];
            else
                reOpt[1][i][j] = 0;

    costValueMLP = C(0, this->dimension);
}

void Solution::update2opt(int a, int b)
{
    if (a > b)
        swap(a, b);

    for (int i = 0; i <= b; i++)
        for (int j = max(a, i); j < size; j++)
            if (i != j)
                reOpt[0][i][j] = reOpt[0][i][j - 1] + in->distanceGet(location[j - 1], location[j]);
            else
                reOpt[0][j][j] = 0;

    for (int i = a; i < size; i++)
        for (int j = min(b, i); j >= 0; j--)
            if (i != j)
                reOpt[0][i][j] = reOpt[0][i][j + 1] + in->distanceGet(location[j], location[j + 1]);
            else
                reOpt[0][j][j] = 0;

    for (int i = 0; i <= b; i++)
        for (int j = max(a, i); j < size; j++)
            if (i != j)
                reOpt[1][i][j] = reOpt[1][i][j - 1] + reOpt[0][i][j];
            else
                reOpt[1][i][j] = 0;

    for (int i = a; i < size; i++)
        for (int j = min(b, i); j >= 0; j--)
            if (i != j)
                reOpt[1][i][j] = reOpt[1][i][j + 1] + reOpt[0][i][j];
            else
                reOpt[1][i][j] = 0;

    //this->printDuration();

    costValueMLP = C(0, this->dimension);
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
            cout << setw(6) << reOpt[1][i][j];
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
            cout << setw(5) << reOpt[0][i][j];
        }
        cout << endl;
    }
}

Solution::Solution(Solution &other)
{
    if (size != other.size)
    {
        this->size = other.size;
        reOpt = new int **[2];
        reOpt[0] = new int *[size];
        cost = new int *[size];
        for (int i = 0; i < size; i++)
        {
            reOpt[0][i] = new int[size];
            reOpt[1][i] = new int[size];
            reOpt[0][i][i] = 0;
            reOpt[1][i][i] = 0;
        }
    }
    this->location = other.location;
    this->costValueMLP = other.costValueMLP;
    this->size = other.size;
    this->dimension = other.dimension;

    for (int i = 0; i < size; i++)
        std::copy(other.reOpt[1][i], other.reOpt[1][i] + other.size, reOpt[1][i]);
    for (int i = 0; i < size; i++)
        std::copy(other.reOpt[0][i], other.reOpt[0][i] + other.size, reOpt[0][i]);
}

Solution &Solution::operator=(const Solution &other) // copy assignment
{
    if (this != &other)
    { // self-assignment check expected
        if (other.size != size)
        { // storage cannot be reused
            cout << "I will not deal with this kind of situation!" << endl;
            cout << __FILE__ << ":" << __LINE__ << endl;
            exit(0);
            // delete[] mArray; // destroy storage in this
            // size = 0;
            // mArray = nullptr;             // preserve invariants in case next line throws
            // mArray = new int[other.size]; // create storage in this
            // size = other.size;
        }
        this->location = other.location;
        this->costValueMLP = other.costValueMLP;
        this->size = other.size;
        this->dimension = other.dimension;
        for (int i = 0; i < size; i++)
            std::copy(other.reOpt[1][i], other.reOpt[1][i] + other.size, reOpt[1][i]);
        for (int i = 0; i < size; i++)
            std::copy(other.reOpt[0][i], other.reOpt[0][i] + other.size, reOpt[0][i]);
    }
    return *this;
}

ostream &operator<<(ostream &out, const Solution &s)
{
    Color::Modifier red(Color::FG_RED);
    Color::Modifier deff(Color::FG_DEFAULT);

    if (s.size <= s.dimension)
        out << red << "Warning! Incomplete solution!!!" << deff << endl;

    out << "Dimension: " << s.dimension << endl;
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