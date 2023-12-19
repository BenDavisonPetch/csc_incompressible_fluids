#include <FVMCode/exceptions.h>
#include <FVMCode/input.h>

namespace FVMCode
{
namespace Input
{

bool comment_istream::skip_one_comment ()
{
    is >> std::ws;
    if (is.peek () != '/')
        return false;

    char next_char;
    is >> next_char;
    // single line comment
    if (is.peek () == '/')
    {
        is.ignore (std::numeric_limits<std::streamsize>::max (), '\n');
        is >> std::ws;
        return true;
    }
    // big comment
    else if (is.peek () == '*')
    {
        while (is.peek () != EOF)
        {
            is.ignore (std::numeric_limits<std::streamsize>::max (), '*');
            is >> std::ws;
            if (is.peek () == EOF)
                return true;
            char next_char;
            is >> next_char;
            if (next_char == '/')
                return true;
        }
        Assert (false, "Error: input stream has unfinished block comment");
        return false;
    }
    else
    {
        is.seekg (-1, is.cur);
        return false;
    }
}

void comment_istream::skip_comments ()
{
    while (skip_one_comment ())
        ;
}

void comment_istream::skip_comments_and_progress ()
{
    skip_comments ();
    if (is.peek () == '/')
    {
        is.seekg (1, is.cur);
    }
    is >> std::ws;
}

comment_istream &operator>> (comment_istream &cis, double &v)
{
    while (cis.is.peek () != EOF)
    {
        // if next character is a digit we read the number
        if (std::isdigit (cis.is.peek ()))
        {
            cis.is >> v;
            return cis;
        }
        // if it is a -ve sign we check if the character after is a digit, if
        // so read the number
        else if (cis.is.peek () == '-')
        {
            cis.is.seekg (1, cis.is.cur);
            if (std::isdigit (cis.is.peek ()))
            {
                cis.is.seekg (-1, cis.is.cur);
                cis.is >> v;
                return cis;
            }
        }
        // if next input is a comment we skip ahead
        cis.skip_comments_and_progress ();
    }
    return cis;
}

comment_istream &operator>> (comment_istream &cis, unsigned int &v)
{
    while (cis.is.peek () != EOF)
    {
        // if next character is a digit we read the number
        if (std::isdigit (cis.is.peek ()))
        {
            cis.is >> v;
            return cis;
        }
        // if next input is a comment we skip ahead
        cis.skip_comments_and_progress ();
    }
    return cis;
}

comment_istream &operator>> (comment_istream &cis, int &v)
{
    while (cis.is.peek () != EOF)
    {
        // if next character is a digit we read the number
        if (std::isdigit (cis.is.peek ()))
        {
            cis.is >> v;
            return cis;
        }
        // if it is a -ve sign we check if the character after is a digit, if
        // so read the number
        else if (cis.is.peek () == '-')
        {
            cis.is.seekg (1, cis.is.cur);
            if (std::isdigit (cis.is.peek ()))
            {
                cis.is.seekg (-1, cis.is.cur);
                cis.is >> v;
                return cis;
            }
        }
        // if next input is a comment we skip ahead
        cis.skip_comments_and_progress ();
    }
    return cis;
}
comment_istream &operator>> (comment_istream &cis, std::string &v)
{
    cis.skip_comments ();
    cis.is >> v;
    return cis;
}

comment_istream &operator>> (comment_istream &cis, char &v)
{
    cis.skip_comments ();
    cis.is >> v;
    return cis;
}

} // namespace Input
} // namespace FVMCode