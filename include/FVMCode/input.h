#ifndef INPUT_H
#define INPUT_H

#include "exceptions.h"

#include <fstream>
#include <iostream>
#include <limits>
#include <type_traits>

namespace FVMCode
{
namespace Input
{

class comment_istream;

comment_istream &operator>> (comment_istream &cis, double &v);
comment_istream &operator>> (comment_istream &cis, unsigned int &v);
comment_istream &operator>> (comment_istream &cis, int &v);
comment_istream &operator>> (comment_istream &cis, std::string &v);
comment_istream &operator>> (comment_istream &cis, char &v);

class comment_istream
{
  public:
    comment_istream (std::string filename)
        : is (filename)
    {
        Assert (is.is_open (), "File is not open!");
    }

    ~comment_istream () { is.close (); }

    int peek () { return is.peek (); }

    void reset () { is.seekg (0, is.beg); }

    friend comment_istream &operator>> (comment_istream &cis, double &v);
    friend comment_istream &operator>> (comment_istream &cis, unsigned int &v);
    friend comment_istream &operator>> (comment_istream &cis, int &v);
    friend comment_istream &operator>> (comment_istream &cis, std::string &v);
    friend comment_istream &operator>> (comment_istream &cis, char &v);

  private:
    std::ifstream is;

    /**
     * If the next character is the start of a comment (i.e. the first
     * character of a // or /\*), skip the stream to the end of the commend,
     * otherwise do nothing
     *
     * Returns true if a comment was skipped and false otherwise.
     */
    bool skip_one_comment ();

    /**
     * Skips all comments and leaves stream unchanged if the next character is
     * a single /
     */
    void skip_comments ();

    /**
     * Skips all comments and leaves stream one character ahead if the next
     * character would have been a /
     */
    void skip_comments_and_progress ();
};

} // namespace Input
} // namespace FVMCode

#endif