#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

bool is_ident_short(const char *s) {
    if (!s || !*s) return false;
    if (!((*s >= 'a' && *s <= 'z') || (*s >= 'A' && *s <= 'Z') || *s == '_')) 
        return false;
    while (*++s)
        if (!((*s >= 'a' && *s <= 'z') || (*s >= 'A' && *s <= 'Z') || 
              (*s >= '0' && *s <= '9') || *s == '_'))
            return false;
    return true;
}
// втори вариант

bool isValidIdent(const char *token)
{
    enum
    {
        q0,
        q1,
        e
    } next_state = q0;
    for (; token && *token; ++token)
    {
        switch (next_state)
        {
        case q0:
            if (isalpha(*token) || *token == '_')
                next_state = q1;
            else
                next_state = e;
            break;
        case q1:
            if (isalnum(*token) || *token == '_')
                next_state = q1;
            else
                next_state = e;
            break;
        case e:
            return false;
        }
    }
    return next_state == q1;
}