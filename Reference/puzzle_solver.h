#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <set>
#include <queue>
#include <unordered_set>

#include "puzzle.h"

using namespace std;

class puzzle_solver
{
private:
    /* data */
    puzzle p;
    unordered_set<string> puzzle_results;
    ostream *os;

public:
    void read_puzzle(istream &is);
    void set_output(ostream &os)
    {
        this->os = &os;
    }

    int solve();

private:
    bool heuristic(puzzle &p, bool head = true);

    void ban_edge_around_zero(puzzle &p);
    void prelink_around_threes(puzzle &p);

    void ban_edge_around_one(puzzle &p);
    void ban_edge_around_two(puzzle &p);
    void ban_edge_around_three(puzzle &p);
    void ban_edge_around_point(puzzle &p);
    void ban_point(puzzle &p);

    void link_around_one(puzzle &p);
    void link_around_two(puzzle &p);
    void link_around_three(puzzle &p);
    void link_around_point(puzzle &p);

    bool try_draw(puzzle &p);

    void DFS(puzzle &p);
    void go_with_line(puzzle &p,
                      const int &start_r, const int &start_c,
                      int src_p_r, int src_p_c,
                      int dst_p_r, int dst_p_c);
    void go_without_line(puzzle &p,
                         const int &start_r, const int &start_c,
                         const int &src_p_r, const int &src_p_c,
                         const int &dst_p_r, const int &dst_p_c);
    void draw_line(puzzle p,
                   const int &start_r, const int &start_c,
                   const int &src_p_r, const int &src_p_c,
                   const int &dst_p_r, const int &dst_p_c);
};

void puzzle_solver::read_puzzle(istream &is)
{
    is >> p.cols >> p.rows;
    // read puzzle
    for (size_t row = 0; row < p.rows; row++)
    {
        string line;
        is >> line;
        vector<int> tmp_row;
        for (size_t col = 0; col < p.cols; col++)
        {
            const char c = line[col];
            switch (c)
            {
            case '0':
            case '1':
            case '2':
            case '3':
                tmp_row.push_back(c - '0');
                break;
            default:
                tmp_row.push_back(-1);
                break;
            }
        }
        p.lat.push_back(tmp_row);
    }
    // init connect state table
    for (size_t row = 0; row < p.rows + 1; row++)
    {
        vector<puzzle::edge_state> tmp_row;
        for (size_t col = 0; col < p.cols; col++)
        {
            tmp_row.push_back(puzzle::NOT);
        }
        p.hrz.push_back(tmp_row);
    }
    for (size_t row = 0; row < p.rows; row++)
    {
        vector<puzzle::edge_state> tmp_row;
        for (size_t col = 0; col < p.cols + 1; col++)
        {
            tmp_row.push_back(puzzle::NOT);
        }
        p.vrt.push_back(tmp_row);
    }
    // init banned point
    for (size_t row = 0; row <= p.rows; row++)
    {
        vector<bool> tmp_row;
        for (size_t col = 0; col <= p.cols; col++)
        {
            tmp_row.push_back(false);
        }
        p.banned_point.push_back(tmp_row);
    }
}

int puzzle_solver::solve()
{
    ban_edge_around_zero(p);
    prelink_around_threes(p);
    if (heuristic(p) == false)
    {
        return -1;
    }
    // Heuristic done
    if (p.is_fin() && p.is_correct())
    {
        // Print solution
        const auto result = p.to_string();
        if (puzzle_results.find(result) == puzzle_results.end())
        {
            puzzle_results.insert(result);
            cout << result;
            *os << result;
        }
    }
    DFS(p);
    return puzzle_results.size();
}

bool puzzle_solver::heuristic(puzzle &p, bool ahead)
{
    string last_step = p.to_string();
    string curr_step;
    while (true)
    {
        ban_edge_around_one(p);
        ban_edge_around_two(p);
        ban_edge_around_three(p);
        ban_edge_around_point(p);
        ban_point(p);
        link_around_point(p);
        link_around_three(p);
        link_around_two(p);
        link_around_one(p);
        if (!p.is_correct())
        {
            return false;
        }
        curr_step = p.to_string();
        // run out of normal methods
        if (last_step.compare(curr_step) == 0)
        {
            break;
        }
        else // normal method make sense
        {
            last_step = curr_step;
        }
    }
    // look ahead
    while (ahead == true)
    {
        if (try_draw(p) == false)
        {
            return false;
        }
        else
        {
            // look ahead make sense
            // try normal methods again
            curr_step = p.to_string();
            if (last_step.compare(curr_step) == 0)
            {
                break;
            } 
            last_step = curr_step;
        }
    }
    // look head not useful
    // return to DFS
    return true;
}

void puzzle_solver::ban_edge_around_zero(puzzle &p)
{
    /**
     * . b .
     * b 0 b
     * . b .
     */
    for (size_t row = 0; row < p.rows; row++)
    {
        for (size_t col = 0; col < p.cols; col++)
        {
            if (p.lat[row][col] == 0)
            {
                p.hrz[row][col] = puzzle::BAN;
                p.hrz[row + 1][col] = puzzle::BAN;
                p.vrt[row][col] = puzzle::BAN;
                p.vrt[row][col + 1] = puzzle::BAN;
            }
        }
    }
}

void puzzle_solver::prelink_around_threes(puzzle &p)
{
    /**
     *   . l .
     *     3
     * b . l . b
     *     3
     *   . l .
     */                                         
    for (size_t row = 0; row < p.rows - 1; row++)
    {
        for (size_t col = 0; col < p.cols; col++)
        {
            if (p.lat[row][col] == 3 && p.lat[row + 1][col] == 3)
            {
                p.hrz[row][col] = puzzle::LINKED;
                p.hrz[row + 1][col] = puzzle::LINKED;
                p.hrz[row + 2][col] = puzzle::LINKED;
                if (p.point_can_left(row + 1, col))
                    p.hrz[row + 1][col - 1] = puzzle::BAN;
                if (p.point_can_right(row + 1, col + 1))
                    p.hrz[row + 1][col + 1] = puzzle::BAN;
            }
        }
    }
    /**
     *     b
     * .   .   .
     * l 3 l 3 l
     * .   .   .
     *     b
     */
    for (size_t row = 0; row < p.rows; row++)
    {
        for (size_t col = 0; col < p.cols - 1; col++)
        {
            if (p.lat[row][col] == 3 && p.lat[row][col + 1] == 3)
            {
                p.vrt[row][col] = puzzle::LINKED;
                p.vrt[row][col + 1] = puzzle::LINKED;
                p.vrt[row][col + 2] = puzzle::LINKED;
                if (p.point_can_up(row, col + 1))
                    p.vrt[row - 1][col + 1] = puzzle::BAN;
                if (p.point_can_down(row + 1, col + 1))
                    p.vrt[row + 1][col + 1] = puzzle::BAN;
            }
        }
    }
    /**
     *   b
     * b . l .   .
     *   l 3
     *   .   .   .
     *         3 l
     *   .   . l . b
     *           b
     */
    for (size_t row = 0; row < p.rows - 1; row++)
    {
        for (size_t col = 0; col < p.cols - 1; col++)
        {
            if (p.lat[row][col] == 3 && p.lat[row + 1][col + 1] == 3)
            {
                p.hrz[row][col] = puzzle::LINKED;
                p.vrt[row][col] = puzzle::LINKED;
                p.hrz[row + 2][col + 1] = puzzle::LINKED;
                p.vrt[row + 1][col + 2] = puzzle::LINKED;
                if (p.point_can_up(row, col))
                    p.vrt[row - 1][col] = puzzle::BAN;
                if (p.point_can_left(row, col))
                    p.hrz[row][col - 1] = puzzle::BAN;
                if (p.point_can_down(row + 2, col + 2))
                    p.vrt[row + 2][col + 2] = puzzle::BAN;
                if (p.point_can_right(row + 2, col + 2))
                    p.hrz[row + 2][col + 2] = puzzle::BAN;
            }
        }
    }
    /**
     *           b
     *   .   . l . b
     *         3 l
     *   .   .   .
     *   l 3
     * b . l .   .
     *   b
     */
    for (size_t row = 0; row < p.rows - 1; row++)
    {
        for (size_t col = 0; col < p.cols - 1; col++)
        {
            if (p.lat[row][col + 1] == 3 && p.lat[row + 1][col] == 3)
            {
                p.hrz[row][col + 1] = puzzle::LINKED;
                p.vrt[row][col + 2] = puzzle::LINKED;
                p.hrz[row + 2][col] = puzzle::LINKED;
                p.vrt[row + 1][col] = puzzle::LINKED;
                if (p.point_can_up(row, col + 2))
                    p.vrt[row - 1][col + 2] = puzzle::BAN;
                if (p.point_can_right(row, col + 2))
                    p.hrz[row][col + 2] = puzzle::BAN;
                if (p.point_can_down(row + 2, col))
                    p.vrt[row + 2][col] = puzzle::BAN;
                if (p.point_can_left(row + 2, col))
                    p.hrz[row + 2][col - 1] = puzzle::BAN;
            }
        }
    }
}

void puzzle_solver::ban_edge_around_one(puzzle &p)
{
    /**
     *   x
     * x . b .
     *   b 1
     *   .   .
     */
    for (size_t row = 0; row < p.rows; row++)
    {
        for (size_t col = 0; col < p.cols; col++)
        {
            if (p.lat[row][col] == 1 && p.get_lat_banned_edge(row, col) < 2)
            {
                if (!p.point_can_up(row, col) &&
                    !p.point_can_left(row, col))
                {
                    p.hrz[row][col] = puzzle::BAN;
                    p.vrt[row][col] = puzzle::BAN;
                }
                if (!p.point_can_up(row, col + 1) &&
                    !p.point_can_right(row, col + 1))
                {
                    p.hrz[row][col] = puzzle::BAN;
                    p.vrt[row][col + 1] = puzzle::BAN;
                }
                if (!p.point_can_down(row + 1, col) &&
                    !p.point_can_left(row + 1, col))
                {
                    p.hrz[row + 1][col] = puzzle::BAN;
                    p.vrt[row][col] = puzzle::BAN;
                }
                if (!p.point_can_down(row + 1, col + 1) &&
                    !p.point_can_right(row + 1, col + 1))
                {
                    p.hrz[row + 1][col] = puzzle::BAN;
                    p.vrt[row][col + 1] = puzzle::BAN;
                }
            }
        }
    }
    /**
     * . b .
     * b 1 |
     * . b .
     */
    for (size_t row = 0; row < p.rows; row++)
    {
        for (size_t col = 0; col < p.cols; col++)
        {
            if (p.lat[row][col] == 1 && p.get_lat_banned_edge(row, col) != 3)
            {
                if (p.hrz_has_edge(row, col))
                {
                    p.hrz[row + 1][col] = puzzle::BAN;
                    p.vrt[row][col] = puzzle::BAN;
                    p.vrt[row][col + 1] = puzzle::BAN;
                }
                else if (p.hrz_has_edge(row + 1, col))
                {
                    p.hrz[row][col] = puzzle::BAN;
                    p.vrt[row][col] = puzzle::BAN;
                    p.vrt[row][col + 1] = puzzle::BAN;
                }
                else if (p.vrt_has_edge(row, col))
                {
                    p.hrz[row][col] = puzzle::BAN;
                    p.hrz[row + 1][col] = puzzle::BAN;
                    p.vrt[row][col + 1] = puzzle::BAN;
                }
                else if (p.vrt_has_edge(row, col + 1))
                {
                    p.hrz[row][col] = puzzle::BAN;
                    p.hrz[row + 1][col] = puzzle::BAN;
                    p.vrt[row][col] = puzzle::BAN;
                }
            }
        }
    }
    /**
     *   x
     * - .   .
     *     1 b
     *   . b .
     */
    for (size_t row = 0; row < p.rows; row++)
    {
        for (size_t col = 0; col < p.cols; col++)
        {
            if (p.lat[row][col] == 1 && p.get_lat_banned_edge(row, col) < 2)
            {
                if ((p.point_has_edge_up(row, col) || p.point_has_edge_left(row, col)) &&
                    (!p.point_can_up(row, col) || !p.point_can_left(row, col)))
                {
                    p.hrz[row + 1][col] = puzzle::BAN;
                    p.vrt[row][col + 1] = puzzle::BAN;
                }
                if ((p.point_has_edge_up(row, col + 1) || p.point_has_edge_right(row, col + 1)) &&
                    (!p.point_can_up(row, col + 1) || !p.point_can_right(row, col + 1)))
                {
                    p.hrz[row + 1][col] = puzzle::BAN;
                    p.vrt[row][col] = puzzle::BAN;
                }
                if ((p.point_has_edge_down(row + 1, col) || p.point_has_edge_left(row + 1, col)) &&
                    (!p.point_can_down(row + 1, col) || !p.point_can_left(row + 1, col)))
                {
                    p.hrz[row][col] = puzzle::BAN;
                    p.vrt[row][col + 1] = puzzle::BAN;
                }
                if ((p.point_has_edge_down(row + 1, col + 1) || p.point_has_edge_right(row + 1, col + 1)) &&
                    (!p.point_can_down(row + 1, col + 1) || !p.point_can_right(row + 1, col + 1)))
                {
                    p.hrz[row][col] = puzzle::BAN;
                    p.vrt[row][col] = puzzle::BAN;
                }
            }
        }
    }
}
 
void puzzle_solver::ban_edge_around_two(puzzle &p)
{
    /**
     * . - .
     * b 2 |
     * . b .
     */
    for (size_t row = 0; row < p.rows; row++)
    {
        for (size_t col = 0; col < p.cols; col++)
        {
            if (p.lat[row][col] == 2 && p.get_lat_banned_edge(row, col) != 2)
            {
                if (p.hrz_has_edge(row, col) &&
                    p.vrt_has_edge(row, col))
                {
                    p.hrz[row + 1][col] = puzzle::BAN;
                    p.vrt[row][col + 1] = puzzle::BAN;
                }
                else if (p.hrz_has_edge(row, col) &&
                         p.hrz_has_edge(row + 1, col))
                {
                    p.vrt[row][col] = puzzle::BAN;
                    p.vrt[row][col + 1] = puzzle::BAN;
                }
                else if (p.hrz_has_edge(row, col) &&
                         p.vrt_has_edge(row, col + 1))
                {
                    p.hrz[row + 1][col] = puzzle::BAN;
                    p.vrt[row][col] = puzzle::BAN;
                }
                else if (p.hrz_has_edge(row + 1, col) &&
                         p.vrt_has_edge(row, col))
                {
                    p.hrz[row][col] = puzzle::BAN;
                    p.vrt[row][col + 1] = puzzle::BAN;
                }
                else if (p.hrz_has_edge(row + 1, col) &&
                         p.vrt_has_edge(row, col + 1))
                {
                    p.hrz[row][col] = puzzle::BAN;
                    p.vrt[row][col] = puzzle::BAN;
                }
                else if (p.vrt_has_edge(row, col) &&
                         p.vrt_has_edge(row, col + 1))
                {
                    p.hrz[row][col] = puzzle::BAN;
                    p.hrz[row + 1][col] = puzzle::BAN;
                }
            }
        }
    }
}

void puzzle_solver::ban_edge_around_three(puzzle &p)
{
    /**
     * . - .
     * | 3 |
     * . b .
     */
    for (size_t row = 0; row < p.rows; row++)
    {
        for (size_t col = 0; col < p.cols; col++)
        {
            if (p.lat[row][col] == 3 && p.get_lat_banned_edge(row, col) != 1)
            {
                if (p.hrz_has_edge(row, col) &&
                    p.hrz_has_edge(row + 1, col) &&
                    p.vrt_has_edge(row, col))
                {
                    p.vrt[row][col + 1] = puzzle::BAN;
                }
                else if (p.hrz_has_edge(row, col) &&
                         p.hrz_has_edge(row + 1, col) &&
                         p.vrt_has_edge(row, col + 1))
                {
                    p.vrt[row][col] = puzzle::BAN;
                }
                else if (p.vrt_has_edge(row, col) &&
                         p.vrt_has_edge(row, col + 1) &&
                         p.hrz_has_edge(row, col))
                {
                    p.hrz[row + 1][col] = puzzle::BAN;
                }
                else if (p.vrt_has_edge(row, col) &&
                         p.vrt_has_edge(row, col + 1) &&
                         p.hrz_has_edge(row + 1, col))
                {
                    p.hrz[row][col] = puzzle::BAN;
                }
            }
        }
    }
}

void puzzle_solver::ban_edge_around_point(puzzle &p)
{
    /**
     *   x
     * x . x
     *   b
     */
    for (size_t row = 0; row <= p.rows; row++)
    {
        for (size_t col = 0; col <= p.cols; col++)
        {
            if (!p.point_can_up(row, col) &&
                !p.point_can_down(row, col) &&
                !p.point_can_left(row, col) &&
                p.point_can_right(row, col))
            {
                p.hrz[row][col] = puzzle::BAN;
            }
            else if (!p.point_can_up(row, col) &&
                     !p.point_can_down(row, col) &&
                     p.point_can_left(row, col) &&
                     !p.point_can_right(row, col))
            {
                p.hrz[row][col - 1] = puzzle::BAN;
            }
            else if (!p.point_can_up(row, col) &&
                     p.point_can_down(row, col) &&
                     !p.point_can_left(row, col) &&
                     !p.point_can_right(row, col))
            {
                p.vrt[row][col] = puzzle::BAN;
            }
            else if (p.point_can_up(row, col) &&
                     !p.point_can_down(row, col) &&
                     !p.point_can_left(row, col) &&
                     !p.point_can_right(row, col))
            {
                p.vrt[row - 1][col] = puzzle::BAN;
            }
        }
    }
    /**
     *   b
     * - . b
     *   |
     */
    for (size_t row = 0; row <= p.rows; row++)
    {
        for (size_t col = 0; col <= p.cols; col++)
        {
            if (p.point_has_edge_up(row, col) &&
                p.point_has_edge_down(row, col))
            {
                if (p.point_can_left(row, col))
                    p.hrz[row][col - 1] = puzzle::BAN;
                if (p.point_can_right(row, col))
                    p.hrz[row][col] = puzzle::BAN;
            }
            else if (p.point_has_edge_left(row, col) &&
                     p.point_has_edge_right(row, col))
            {
                if (p.point_can_up(row, col))
                    p.vrt[row - 1][col] = puzzle::BAN;
                if (p.point_can_down(row, col))
                    p.vrt[row][col] = puzzle::BAN;
            }
            else if (p.point_has_edge_up(row, col) &&
                     p.point_has_edge_left(row, col))
            {
                if (p.point_can_right(row, col))
                    p.hrz[row][col] = puzzle::BAN;
                if (p.point_can_down(row, col))
                    p.vrt[row][col] = puzzle::BAN;
            }
            else if (p.point_has_edge_up(row, col) &&
                     p.point_has_edge_right(row, col))
            {
                if (p.point_can_left(row, col))
                    p.hrz[row][col - 1] = puzzle::BAN;
                if (p.point_can_down(row, col))
                    p.vrt[row][col] = puzzle::BAN;
            }
            else if (p.point_has_edge_down(row, col) &&
                     p.point_has_edge_left(row, col))
            {
                if (p.point_can_up(row, col))
                    p.vrt[row - 1][col] = puzzle::BAN;
                if (p.point_can_right(row, col))
                    p.hrz[row][col] = puzzle::BAN;
            }
            else if (p.point_has_edge_down(row, col) &&
                     p.point_has_edge_right(row, col))
            {
                if (p.point_can_up(row, col))
                    p.vrt[row - 1][col] = puzzle::BAN;
                if (p.point_can_left(row, col))
                    p.hrz[row][col - 1] = puzzle::BAN;
            }
        }
    }
}

void puzzle_solver::ban_point(puzzle &p)
{
    /**
     *   x
     * x b x
     *   x
     */
    for (size_t row = 0; row <= p.rows; row++)
    {
        for (size_t col = 0; col <= p.cols; col++)
        {
            if (!p.point_can_up(row, col) &&
                !p.point_can_down(row, col) &&
                !p.point_can_left(row, col) &&
                !p.point_can_right(row, col))
            {
                p.banned_point[row][col] = true;
            }
        }
    }
}

void puzzle_solver::link_around_one(puzzle &p)
{
    /**
     * . x .
     * x 1 x
     * . l .
     */
    for (size_t row = 0; row < p.rows; row++)
    {
        for (size_t col = 0; col < p.cols; col++)
        {
            if (p.lat[row][col] == 1 &&
                !p.complete_lat(row, col) &&
                p.get_lat_banned_edge(row, col) == 3)
            {
                if (p.hrz[row][col] != puzzle::BAN)
                {
                    p.hrz[row][col] = puzzle::LINKED;
                }
                else if (p.hrz[row + 1][col] != puzzle::BAN)
                {
                    p.hrz[row + 1][col] = puzzle::LINKED;
                }
                else if (p.vrt[row][col] != puzzle::BAN)
                {
                    p.vrt[row][col] = puzzle::LINKED;
                }
                else if (p.vrt[row][col + 1] != puzzle::BAN)
                {
                    p.vrt[row][col + 1] = puzzle::LINKED;
                }
            }
        }
    }
}

void puzzle_solver::link_around_two(puzzle &p)
{
    /**
     * . l .
     * x 2 x
     * . l .
     */
    for (size_t row = 0; row < p.rows; row++)
    {
        for (size_t col = 0; col < p.cols; col++)
        {
            if (p.lat[row][col] == 2 &&
                !p.complete_lat(row, col) &&
                p.get_lat_banned_edge(row, col) == 2)
            {
                if (p.hrz[row][col] == puzzle::BAN &&
                    p.hrz[row + 1][col] == puzzle::BAN)
                {
                    p.vrt[row][col] = puzzle::LINKED;
                    p.vrt[row][col + 1] = puzzle::LINKED;
                }
                else if (p.vrt[row][col] == puzzle::BAN &&
                         p.vrt[row][col + 1] == puzzle::BAN)
                {
                    p.hrz[row][col] = puzzle::LINKED;
                    p.hrz[row + 1][col] = puzzle::LINKED;
                }
                else if (p.hrz[row][col] == puzzle::BAN &&
                         p.vrt[row][col] == puzzle::BAN)
                {
                    p.hrz[row + 1][col] = puzzle::LINKED;
                    p.vrt[row][col + 1] = puzzle::LINKED;
                }
                else if (p.hrz[row][col] == puzzle::BAN &&
                         p.vrt[row][col + 1] == puzzle::BAN)
                {
                    p.hrz[row + 1][col] = puzzle::LINKED;
                    p.vrt[row][col] = puzzle::LINKED;
                }
                else if (p.hrz[row + 1][col] == puzzle::BAN &&
                         p.vrt[row][col] == puzzle::BAN)
                {
                    p.hrz[row][col] = puzzle::LINKED;
                    p.vrt[row][col + 1] = puzzle::LINKED;
                }
                else if (p.hrz[row + 1][col] == puzzle::BAN &&
                         p.vrt[row][col + 1] == puzzle::BAN)
                {
                    p.hrz[row][col] = puzzle::LINKED;
                    p.vrt[row][col] = puzzle::LINKED;
                }
            }
        }
    }
    /**
     *       ?
     *   .   . ?
     *     2  
     * ? .   . x
     *   ?   x
     */
    for (size_t row = 0; row < p.rows; row++)
    {
        for (size_t col = 0; col < p.cols; col++)
        {
            if (p.lat[row][col] == 2 &&
                !p.complete_lat(row, col))
            {
                if (!p.point_can_up(row, col) && !p.point_can_left(row, col))
                {
                    if (!p.point_can_left(row + 1, col) && p.point_can_down(row + 1, col))
                        p.vrt[row + 1][col] = puzzle::LINKED;
                    if (p.point_can_left(row + 1, col) && !p.point_can_down(row + 1, col))
                        p.hrz[row + 1][col - 1] = puzzle::LINKED;
                    if (!p.point_can_right(row, col + 1) && p.point_can_up(row, col + 1))
                        p.vrt[row - 1][col + 1] = puzzle::LINKED;
                    if (p.point_can_right(row, col + 1) && !p.point_can_up(row, col + 1))
                        p.hrz[row][col + 1] = puzzle::LINKED;
                }
                if (!p.point_can_up(row, col + 1) && !p.point_can_right(row, col + 1))
                {
                    if (!p.point_can_left(row, col) && p.point_can_up(row, col))
                        p.vrt[row - 1][col] = puzzle::LINKED;
                    if (p.point_can_left(row, col) && !p.point_can_up(row, col))
                        p.hrz[row][col - 1] = puzzle::LINKED;
                    if (!p.point_can_right(row + 1, col + 1) && p.point_can_down(row + 1, col + 1))
                        p.vrt[row + 1][col + 1] = puzzle::LINKED;
                    if (p.point_can_right(row + 1, col + 1) && !p.point_can_down(row + 1, col + 1))
                        p.hrz[row + 1][col + 1] = puzzle::LINKED;
                }
                if (!p.point_can_down(row + 1, col + 1) && !p.point_can_right(row + 1, col + 1))
                {
                    if (!p.point_can_right(row, col + 1) && p.point_can_up(row, col + 1))
                        p.vrt[row - 1][col + 1] = puzzle::LINKED;
                    if (p.point_can_right(row, col + 1) && !p.point_can_up(row, col + 1))
                        p.hrz[row][col + 1] = puzzle::LINKED;
                    if (!p.point_can_left(row + 1, col) && p.point_can_down(row + 1, col))
                        p.vrt[row + 1][col] = puzzle::LINKED;
                    if (p.point_can_left(row + 1, col) && !p.point_can_down(row + 1, col))
                        p.hrz[row + 1][col - 1] = puzzle::LINKED;
                }
                if (!p.point_can_down(row + 1, col) && !p.point_can_left(row + 1, col))
                {
                    if (!p.point_can_left(row, col) && p.point_can_up(row, col))
                        p.vrt[row - 1][col] = puzzle::LINKED;
                    if (p.point_can_left(row, col) && !p.point_can_up(row, col))
                        p.hrz[row][col - 1] = puzzle::LINKED;
                    if (!p.point_can_right(row + 1, col + 1) && p.point_can_down(row + 1, col + 1))
                        p.vrt[row + 1][col + 1] = puzzle::LINKED;
                    if (p.point_can_right(row + 1, col + 1) && !p.point_can_down(row + 1, col + 1))
                        p.hrz[row + 1][col + 1] = puzzle::LINKED;
                }
            }
        }
    }
}

void puzzle_solver::link_around_three(puzzle &p)
{
    /**
     * . l .
     * x 3 l
     * . l .
     */
    for (size_t row = 0; row < p.rows; row++)
    {
        for (size_t col = 0; col < p.cols; col++)
        {
            if (p.lat[row][col] == 3 &&
                !p.complete_lat(row, col) &&
                p.get_lat_banned_edge(row, col) == 1)
            {
                if (p.hrz[row][col] == puzzle::BAN)
                {
                    p.hrz[row + 1][col] = puzzle::LINKED;
                    p.vrt[row][col] = puzzle::LINKED;
                    p.vrt[row][col + 1] = puzzle::LINKED;
                }
                else if (p.hrz[row + 1][col] == puzzle::BAN)
                {
                    p.hrz[row][col] = puzzle::LINKED;
                    p.vrt[row][col] = puzzle::LINKED;
                    p.vrt[row][col + 1] = puzzle::LINKED;
                }
                else if (p.vrt[row][col] == puzzle::BAN)
                {
                    p.hrz[row][col] = puzzle::LINKED;
                    p.hrz[row + 1][col] = puzzle::LINKED;
                    p.vrt[row][col + 1] = puzzle::LINKED;
                }
                else if (p.vrt[row][col + 1] == puzzle::BAN)
                {
                    p.hrz[row][col] = puzzle::LINKED;
                    p.hrz[row + 1][col] = puzzle::LINKED;
                    p.vrt[row][col] = puzzle::LINKED;
                }
            }
        }
    }
    /**
     *   x
     * x . l .
     *   l 3
     *   .   .
     */
    for (size_t row = 0; row < p.rows; row++)
    {
        for (size_t col = 0; col < p.cols; col++)
        {
            if (p.lat[row][col] == 3 && !p.complete_lat(row, col))
            {
                if (!p.point_can_up(row, col) &&
                    !p.point_can_left(row, col))
                {
                    p.hrz[row][col] = puzzle::LINKED;
                    p.vrt[row][col] = puzzle::LINKED;
                }
                if (!p.point_can_up(row, col + 1) &&
                    !p.point_can_right(row, col + 1))
                {
                    p.hrz[row][col] = puzzle::LINKED;
                    p.vrt[row][col + 1] = puzzle::LINKED;
                }
                if (!p.point_can_down(row + 1, col) &&
                    !p.point_can_left(row + 1, col))
                {
                    p.hrz[row + 1][col] = puzzle::LINKED;
                    p.vrt[row][col] = puzzle::LINKED;
                }
                if (!p.point_can_down(row + 1, col + 1) &&
                    !p.point_can_right(row + 1, col + 1))
                {
                    p.hrz[row + 1][col] = puzzle::LINKED;
                    p.vrt[row][col + 1] = puzzle::LINKED;
                }
            }
        }
    }
    /**
     * - .   .
     *     3 l
     *   . l .
     */
    for (size_t row = 0; row < p.rows; row++)
    {
        for (size_t col = 0; col < p.cols; col++)
        {
            if (p.lat[row][col] == 3 && !p.complete_lat(row, col))
            {
                if (p.point_has_edge_up(row, col) ||
                    p.point_has_edge_left(row, col))
                {
                    if (p.hrz[row + 1][col] == puzzle::NOT &&
                        p.vrt[row][col + 1] == puzzle::NOT)
                    {
                        p.hrz[row + 1][col] = puzzle::LINKED;
                        p.vrt[row][col + 1] = puzzle::LINKED;
                    }
                }
                if (p.point_has_edge_up(row, col + 1) ||
                    p.point_has_edge_right(row, col + 1))
                {
                    if (p.hrz[row + 1][col] == puzzle::NOT &&
                        p.vrt[row][col] == puzzle::NOT)
                    {
                        p.hrz[row + 1][col] = puzzle::LINKED;
                        p.vrt[row][col] = puzzle::LINKED;
                    }
                }
                if (p.point_has_edge_down(row + 1, col) ||
                    p.point_has_edge_left(row + 1, col))
                {
                    if (p.hrz[row][col] == puzzle::NOT &&
                        p.vrt[row][col + 1] == puzzle::NOT)
                    {
                        p.hrz[row][col] = puzzle::LINKED;
                        p.vrt[row][col + 1] = puzzle::LINKED;
                    }
                }
                if (p.point_has_edge_down(row + 1, col + 1) ||
                    p.point_has_edge_right(row + 1, col + 1))
                {
                    if (p.hrz[row][col] == puzzle::NOT &&
                        p.vrt[row][col] == puzzle::NOT)
                    {
                        p.hrz[row][col] = puzzle::LINKED;
                        p.vrt[row][col] = puzzle::LINKED;
                    }
                }
            }
        }
    }
}

void puzzle_solver::link_around_point(puzzle &p)
{
    /**
     *   x
     * l . x
     *   |
     */
    for (size_t row = 0; row <= p.rows; row++)
    {
        for (size_t col = 0; col <= p.cols; col++)
        {
            if (p.get_conn(row, col) == 1)
            {
                if (p.point_can_up(row, col) &&
                    p.point_can_down(row, col) &&
                    !p.point_can_left(row, col) &&
                    !p.point_can_right(row, col))
                {
                    p.vrt[row - 1][col] = puzzle::LINKED;
                    p.vrt[row][col] = puzzle::LINKED;
                }
                else if (p.point_can_up(row, col) &&
                         !p.point_can_down(row, col) &&
                         p.point_can_left(row, col) &&
                         !p.point_can_right(row, col))
                {
                    p.vrt[row - 1][col] = puzzle::LINKED;
                    p.hrz[row][col - 1] = puzzle::LINKED;
                }
                else if (p.point_can_up(row, col) &&
                         !p.point_can_down(row, col) &&
                         !p.point_can_left(row, col) &&
                         p.point_can_right(row, col))
                {
                    p.vrt[row - 1][col] = puzzle::LINKED;
                    p.hrz[row][col] = puzzle::LINKED;
                }
                else if (!p.point_can_up(row, col) &&
                         p.point_can_down(row, col) &&
                         p.point_can_left(row, col) &&
                         !p.point_can_right(row, col))
                {
                    p.vrt[row][col] = puzzle::LINKED;
                    p.hrz[row][col - 1] = puzzle::LINKED;
                }
                else if (!p.point_can_up(row, col) &&
                         p.point_can_down(row, col) &&
                         !p.point_can_left(row, col) &&
                         p.point_can_right(row, col))
                {
                    p.vrt[row][col] = puzzle::LINKED;
                    p.hrz[row][col] = puzzle::LINKED;
                }
                else if (!p.point_can_up(row, col) &&
                         !p.point_can_down(row, col) &&
                         p.point_can_left(row, col) &&
                         p.point_can_right(row, col))
                {
                    p.hrz[row][col - 1] = puzzle::LINKED;
                    p.hrz[row][col] = puzzle::LINKED;
                }
            }
        }
    }
}

bool puzzle_solver::try_draw(puzzle &p)
{
    // horizontal
    for (size_t row = 0; row <= p.rows; row++)
    {
        for (size_t col = 1; col < p.cols; col++)
        {
            if (p.hrz[row][col] == puzzle::NOT)
            {
                puzzle np = p;
                bool no_link = false;
                bool no_ban = false;
                np.hrz[row][col] = puzzle::BAN;
                if (heuristic(np, false) == false)
                {
                    no_ban = true;
                }
                np = p;
                np.hrz[row][col] = puzzle::LINKED;
                if (heuristic(np, false) == false)
                {
                    no_link = true;
                }
                if (no_link && no_ban)
                {
                    return false;
                }
                else if (no_link)
                {
                    p.hrz[row][col] = puzzle::BAN;
                    heuristic(p, false);
                }
                else if (no_ban)
                {
                    p.hrz[row][col] = puzzle::LINKED;
                    heuristic(p, false);
                }
            }
        }
    }
    // vertical
    for (size_t row = 0; row < p.rows; row++)
    {
        for (size_t col = 1; col <= p.cols; col++)
        {
            if (p.vrt[row][col] == puzzle::NOT)
            {
                puzzle np = p;
                bool no_link = false;
                bool no_ban = false;
                np.vrt[row][col] = puzzle::BAN;
                if (heuristic(np, false) == false)
                {
                    no_ban = true;
                }
                np = p;
                np.vrt[row][col] = puzzle::LINKED;
                if (heuristic(np, false) == false)
                {
                    no_link = true;
                }
                if (no_link && no_ban)
                {
                    return false;
                }
                else if (no_link)
                {
                    p.vrt[row][col] = puzzle::BAN;
                    heuristic(p, false);
                }
                else if (no_ban)
                {
                    p.vrt[row][col] = puzzle::LINKED;
                    heuristic(p, false);
                }
            }
        }
    }
    // around two
    for (size_t row = 0; row < p.rows; row++)
    {
        for (size_t col = 1; col < p.cols; col++)
        {
            if (p.lat[row][col] == 2 &&
                p.lat_edge(row, col) == 0 &&
                p.get_lat_banned_edge(row, col) == 0)
            {
                puzzle np = p;
                int not_allow[6][4] = {0};
                np.hrz[row][col] = puzzle::LINKED;
                np.vrt[row][col] = puzzle::LINKED;
                if (heuristic(np, false) == false)
                {
                    not_allow[0][0] = -1;
                    not_allow[0][1] = -1;
                    not_allow[0][2] = 1;
                    not_allow[0][3] = 1;
                }
                np = p;
                np.hrz[row][col] = puzzle::LINKED;
                np.vrt[row][col + 1] = puzzle::LINKED;
                if (heuristic(np, false) == false)
                {
                    not_allow[1][0] = 1;
                    not_allow[1][1] = -1;
                    not_allow[1][2] = -1;
                    not_allow[1][3] = 1;
                }
                np = p;
                np.hrz[row + 1][col] = puzzle::LINKED;
                np.vrt[row][col + 1] = puzzle::LINKED;
                if (heuristic(np, false) == false)
                {
                    not_allow[2][0] = 1;
                    not_allow[2][1] = 1;
                    not_allow[2][2] = -1;
                    not_allow[2][3] = -1;
                }
                np = p;
                np.hrz[row + 1][col] = puzzle::LINKED;
                np.vrt[row][col] = puzzle::LINKED;
                if (heuristic(np, false) == false)
                {
                    not_allow[3][0] = -1;
                    not_allow[3][1] = 1;
                    not_allow[3][2] = 1;
                    not_allow[3][3] = -1;
                }
                np = p;
                np.hrz[row][col] = puzzle::LINKED;
                np.hrz[row + 1][col] = puzzle::LINKED;
                if (heuristic(np, false) == false)
                {
                    not_allow[4][0] = 1;
                    not_allow[4][1] = -1;
                    not_allow[4][2] = 1;
                    not_allow[4][3] = -1;
                }
                np = p;
                np.vrt[row][col] = puzzle::LINKED;
                np.vrt[row][col + 1] = puzzle::LINKED;
                if (heuristic(np, false) == false)
                {
                    not_allow[5][0] = -1;
                    not_allow[5][1] = 1;
                    not_allow[5][2] = -1;
                    not_allow[5][3] = 1;
                }
                int no_cnt[4] = {0};
                int link_cnt[4] = {0};
                for (size_t i = 0; i < 6; i++)
                {
                    for (size_t j = 0; j < 4; j++)
                    {
                        no_cnt[j] += (not_allow[i][j] == -1 ? 1 : 0);
                        link_cnt[j] += (not_allow[i][j] == 1 ? 1 : 0);
                    }
                }
                if (no_cnt[0] == 3)
                {
                    p.vrt[row][col] = puzzle::BAN;
                }
                if (no_cnt[1] == 3)
                {
                    p.hrz[row][col] = puzzle::BAN;
                }
                if (no_cnt[2] == 3)
                {
                    p.vrt[row][col + 1] = puzzle::BAN;
                }
                if (no_cnt[3] == 3)
                {
                    p.hrz[row + 1][col] = puzzle::BAN;
                }
                if (link_cnt[0] == 3)
                {
                    p.vrt[row][col] = puzzle::LINKED;
                }
                if (link_cnt[1] == 3)
                {
                    p.hrz[row][col] = puzzle::LINKED;
                }
                if (link_cnt[2] == 3)
                {
                    p.vrt[row][col + 1] = puzzle::LINKED;
                }
                if (link_cnt[3] == 3)
                {
                    p.hrz[row + 1][col] = puzzle::LINKED;
                }
                heuristic(p, false);
            }
        }
    }
    return true;
}

void puzzle_solver::DFS(puzzle &p)
{
    // Start with one line
    for (size_t row = 0; row <= p.rows; row++)
    {
        for (size_t col = 1; col <= p.cols; col++)
        {
            if (!p.banned_point[row][col] && p.get_conn(row, col) == 1)
            {
                go_without_line(p, row, col, row, col, row, col);
                return;
            }
        }
    }
    // No line on this map
    // For every points find all solutions
    for (size_t row = 0; row <= p.rows; row++)
    {
        // Optimized
        for (size_t col = 1; col <= p.cols; col += 2)
        {
            go_without_line(p, row, col, row, col, row, col);
            // set point banned
            p.banned_point[row][col - 1] = true;
            p.banned_point[row][col] = true;
            if (col + 1 == p.cols)
                p.banned_point[row][col + 1] = true;
        }
    }
}

void puzzle_solver::go_with_line(puzzle &p,
                                 const int &start_r, const int &start_c,
                                 int src_p_r, int src_p_c,
                                 int dst_p_r, int dst_p_c)
{
    while (p.get_conn(dst_p_r, dst_p_c) == 2) // with line
    {
        // If solved
        if (dst_p_r == start_r && dst_p_c == start_c)
        {
            // Do final check
            if (p.is_fin() && p.is_correct())
            {
                // Print solution
                const auto result = p.to_string();
                if (puzzle_results.find(result) == puzzle_results.end())
                {
                    puzzle_results.insert(result);
                    cout << result;
                    *os << result;
                }
            }
            return;
        }
        // one step
        if (p.point_can_up(dst_p_r, dst_p_c) &&
            p.vrt_has_edge(dst_p_r - 1, dst_p_c) &&
            dst_p_r - 1 != src_p_r)
        {
            src_p_r = dst_p_r;
            src_p_c = dst_p_c;
            --dst_p_r;
        }
        else if (p.point_can_down(dst_p_r, dst_p_c) &&
                 p.vrt_has_edge(dst_p_r, dst_p_c) &&
                 dst_p_r + 1 != src_p_r)
        {
            src_p_r = dst_p_r;
            src_p_c = dst_p_c;
            ++dst_p_r;
        }
        else if (p.point_can_left(dst_p_r, dst_p_c) &&
                 p.hrz_has_edge(dst_p_r, dst_p_c - 1) &&
                 dst_p_c - 1 != src_p_c)
        {
            src_p_r = dst_p_r;
            src_p_c = dst_p_c;
            --dst_p_c;
        }
        else if (p.point_can_right(dst_p_r, dst_p_c) &&
                 p.hrz_has_edge(dst_p_r, dst_p_c) &&
                 dst_p_c + 1 != src_p_c)
        {
            src_p_r = dst_p_r;
            src_p_c = dst_p_c;
            ++dst_p_c;
        }
    }
    // without line
    go_without_line(p,
                    start_r, start_c,
                    src_p_r, src_p_c,
                    dst_p_r, dst_p_c);
}

void puzzle_solver::go_without_line(puzzle &p,
                                    const int &start_r, const int &start_c,
                                    const int &src_p_r, const int &src_p_c,
                                    const int &dst_p_r, const int &dst_p_c)
{
    if (p.point_can_up(dst_p_r, dst_p_c) &&
        p.vrt[dst_p_r - 1][dst_p_c] == puzzle::NOT) // try draw up
    {
        draw_line(p,
                  start_r, start_c,
                  dst_p_r, dst_p_c,
                  dst_p_r - 1, dst_p_c);
    }
    if (p.point_can_down(dst_p_r, dst_p_c) &&
        p.vrt[dst_p_r][dst_p_c] == puzzle::NOT) // try draw down
    {
        draw_line(p,
                  start_r, start_c,
                  dst_p_r, dst_p_c,
                  dst_p_r + 1, dst_p_c);
    }
    if (p.point_can_left(dst_p_r, dst_p_c) &&
        p.hrz[dst_p_r][dst_p_c - 1] == puzzle::NOT) // try draw left
    {
        draw_line(p,
                  start_r, start_c,
                  dst_p_r, dst_p_c,
                  dst_p_r, dst_p_c - 1);
    }
    if (p.point_can_right(dst_p_r, dst_p_c) &&
        p.hrz[dst_p_r][dst_p_c] == puzzle::NOT) // try draw right
    {
        draw_line(p,
                  start_r, start_c,
                  dst_p_r, dst_p_c,
                  dst_p_r, dst_p_c + 1);
    }
}

void puzzle_solver::draw_line(puzzle p,
                              const int &start_r, const int &start_c,
                              const int &src_p_r, const int &src_p_c,
                              const int &dst_p_r, const int &dst_p_c)
{
    if (src_p_r == dst_p_r) // previous go horizontally
    {
        int h_r, h_c;
        if (src_p_c > dst_p_c) // previous go left
        {
            h_r = dst_p_r;
            h_c = dst_p_c;
        }
        else // previous go right
        {
            h_r = src_p_r;
            h_c = src_p_c;
        }
        // Draw line
        p.hrz[h_r][h_c] = puzzle::LINKED;
        // Check lattice
        if (!p.hrz_sat(h_r, h_c))
            return;
    }
    else // previous go vertically
    {
        int v_r, v_c;
        if (src_p_r > dst_p_r) // previous go up
        {
            v_r = dst_p_r;
            v_c = dst_p_c;
        }
        else // previous go down
        {
            v_r = src_p_r;
            v_c = src_p_c;
        }
        // Draw line
        p.vrt[v_r][v_c] = puzzle::LINKED;
        // Check lattice
        if (!p.vrt_sat(v_r, v_c))
            return;
    }
    // Check connectivity
    if (p.get_conn(dst_p_r, dst_p_c) > 2 || p.get_conn(dst_p_r, dst_p_c) == 0)
        return;
    // Do heuristic
    if (heuristic(p) == false)
        return;
    // If solved
    if (dst_p_r == start_r && dst_p_c == start_c)
    {
        // Do final check
        if (p.is_fin() && p.is_correct())
        {
            // Print solution
            const auto result = p.to_string();
            if (puzzle_results.find(result) == puzzle_results.end())
            {
                puzzle_results.insert(result);
                cout << result;
                *os << result;
            }
        }
        return;
    }
    // Go next point
    if (p.get_conn(dst_p_r, dst_p_c) == 2)
    {
        go_with_line(p, start_r, start_c, src_p_r, src_p_c, dst_p_r, dst_p_c);
    }
    else // 1
    {
        go_without_line(p, start_r, start_c, src_p_r, src_p_c, dst_p_r, dst_p_c);
    }
}
