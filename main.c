#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/*
    the following functions are replaced:
    ungetc() -> b_ungetc()
    fgetc() -> b_fgetc()

    the reason: undefined behavior of function ungetc() in the case of pushing back multiple characters.
    functions b_ungetc() and b_fgetc() are using buffer[20] to manage the multiple-character push back.

    function ungets() is used to push back string into buffer[].
*/
char temp[2] = {'v', '\0'}; // just used in concatenation.
int size = 0; // number of characters currently inside buffer.
char buffer[20] = "";
char b_ungetc(char c)
{
    buffer[size] = c;
    size++;
    return c;
}

char b_fgetc(FILE *fp)
{
    if (size > 0)
    {
        return buffer[--size];
    }

    return fgetc(fp);
}

void ungets(char str[], FILE *fp)
{
    int len = strlen(str);
    for (int i = len ; i > 0 ; i--)
    {
        b_ungetc(str[i - 1]);
        ("ungetted: %c\n", str[i -1]);
    }

    str[0] = '\0';
}

char* scan_token(FILE *fp)
{
    char str[20] = "";
    int c = b_fgetc(fp);
    while (c == ' ')
    {
        c = b_fgetc(fp);
    }

    if (c == EOF)
    {
        return "EOF";
    }
    else if (c == 10)
    {
        return "\n";
    }
    else if (c == '$')
    {
        c = b_fgetc(fp);
        if (c == EOF)
        {
            return "<ERROR>";
        }
        else if (c == '$') //multiple-line comment
        {
            while (1)
            {
                do
                {
                    c = b_fgetc(fp);
                }
                while ((c != '$') && (c != EOF));
                if (c == EOF)
                {
                    return "<ERROR>";
                }
                else if (c == '$')
                {
                    c = b_fgetc(fp);
                    if (c == EOF)
                    {
                        return "<ERROR>";
                    }
                    else if (c == '$')
                    {
                        return scan_token(fp);
                    }
                }
            }
        }
        else //single-line comment
        {
            while ((c != '$') && (c != 10) && (c != EOF))
            {
                c = b_fgetc(fp);
            }

            if (c == '$')
            {
                return scan_token(fp);
            }
            else
            {
                return "<ERROR>";
            }
        }
    }
    else if (c == 'i') // data type i
    {
        temp[0] = c;
        strcat(str, temp);

        c = b_fgetc(fp);
        temp[0] = c;
        strcat(str, temp);
        if (c == '8')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);

            if (!isalnum(c))
            {
                b_ungetc(c);
                return "<i8>";
            }
        }
        else if (c == '1')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);

            if (c == '6')
            {
                c = b_fgetc(fp);

                temp[0] = c;
                strcat(str, temp);

                if (!isalnum(c))
                {
                    b_ungetc(c);
                    return "<i16>";
                }
            }
            else if (c == '2')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);

                if (c == '8')
                {
                    c = b_fgetc(fp);

                    temp[0] = c;
                    strcat(str, temp);

                    if (!isalnum(c))
                    {
                        b_ungetc(c);
                        return "<i128>";
                    }
                }
            }
        }
        else if (c == '6')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);


            if (c == '4')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);

                if (!isalnum(c))
                {
                    b_ungetc(c);
                    return "<i64>";
                }
            }
        }
        else if (c == '3')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);


            if (c == '2')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);

                if (!isalnum(c))
                {
                    b_ungetc(c);
                    return "<i32>";
                }
            }
        }
        else if (c == 's')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);


            if (c == 'i')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);


                if (c == 'z')
                {
                    c = b_fgetc(fp);
                    temp[0] = c;
                    strcat(str, temp);


                    if (c == 'e')
                    {
                        c = b_fgetc(fp);
                        temp[0] = c;
                        strcat(str, temp);

                        if (!isalnum(c))
                        {
                            b_ungetc(c);
                            return "<isize>";
                        }
                    }
                }
            }
        }
        ungets(str, fp);
        c = b_fgetc(fp);
    }
    if (c == 'u') // data type u
    {
        temp[0] = c;
        strcat(str, temp);
        c = b_fgetc(fp);
        temp[0] = c;
        strcat(str, temp);

        if (c == '8')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);

            if (!isalnum(c))
            {
                b_ungetc(c);
                return "<u8>";
            }
        }
        else if (c == '1')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);

            if (c == '6')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);

                if (!isalnum(c))
                {
                    b_ungetc(c);
                    return "<u16>";
                }
            }
            else if (c == '2')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);


                if (c == '8')
                {
                    c = b_fgetc(fp);
                    temp[0] = c;
                    strcat(str, temp);

                    if (!isalnum(c))
                    {
                        b_ungetc(c);
                        return "<u128>";
                    }
                }
            }
        }
        else if (c == '6')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);


            if (c == '4')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);

                if (!isalnum(c))
                {
                    b_ungetc(c);
                    return "<u64>";
                }
            }
        }
        else if (c == '3')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);


            if (c == '2')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);

                if (!isalnum(c))
                {
                    b_ungetc(c);
                    return "<u32>";
                }
            }
        }
        else if (c == 's')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);


            if (c == 'i')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);


                if (c == 'z')
                {
                    c = b_fgetc(fp);
                    temp[0] = c;
                    strcat(str, temp);

                    if (c == 'e')
                    {
                        c = b_fgetc(fp);
                        temp[0] = c;
                        strcat(str, temp);

                        if (!isalnum(c))
                        {
                            b_ungetc(c);
                            return "<usize>";
                        }
                    }
                }
            }
        }
        ungets(str, fp);
        c = b_fgetc(fp);
    }
    if (c == 'f') // data type f
    {
        temp[0] = c;
        strcat(str, temp);
        c = b_fgetc(fp);
        temp[0] = c;
        strcat(str, temp);

        if (c == '8')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);

            if (!isalnum(c))
            {
                b_ungetc(c);
                return "<f8>";
            }
        }
        else if (c == '1')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);

            if (c == '6')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);

                if (!isalnum(c))
                {
                    b_ungetc(c);
                    return "<f16>";
                }
            }
            else if (c == '2')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);


                if (c == '8')
                {
                    c = b_fgetc(fp);
                    temp[0] = c;
                    strcat(str, temp);

                    if (!isalnum(c))
                    {
                        b_ungetc(c);
                        return "<f128>";
                    }
                }
            }
        }
        else if (c == '6')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);


            if (c == '4')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);

                if (!isalnum(c))
                {
                    b_ungetc(c);
                    return "<f64>";
                }
            }
        }
        else if (c == '3')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);


            if (c == '2')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);

                if (!isalnum(c))
                {
                    b_ungetc(c);
                    return "<f32>";
                }
            }
        }
        else if (c == 's')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);


            if (c == 'i')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);


                if (c == 'z')
                {
                    c = b_fgetc(fp);
                    temp[0] = c;
                    strcat(str, temp);


                    if (c == 'e')
                    {
                        c = b_fgetc(fp);
                        temp[0] = c;
                        strcat(str, temp);

                        if (!isalnum(c))
                        {
                            b_ungetc(c);
                            return "<fsize>";
                        }
                    }
                }
            }
        }
        ungets(str, fp);
        c = b_fgetc(fp);
    }
    if (c == 'i')
    {
        temp[0] = c;
        strcat(str, temp);

        c = b_fgetc(fp);
        temp[0] = c;
        strcat(str, temp);


        if (c == 'f')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);

            if (!isalnum(c))
            {
                b_ungetc(c);
                return "<if>";
            }
        }
        ungets(str, fp);
        c = b_fgetc(fp);
    }
    if (c == 'o')
    {
        temp[0] = c;
        strcat(str, temp);

        c = b_fgetc(fp);
        temp[0] = c;
        strcat(str, temp);


        if (c == 'w')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);

            if (!isalnum(c))
            {
                b_ungetc(c);
                return "<otherwise>";
            }
        }
        ungets(str, fp);
        c = b_fgetc(fp);
    }

    if (c == 'u') // until
    {
        temp[0] = c;
        strcat(str, temp);
        c = b_fgetc(fp);
        temp[0] = c;
        strcat(str, temp);


        if (c == 'n')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);


            if (c == 't')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);


                if (c == 'i')
                {
                    c = b_fgetc(fp);
                    temp[0] = c;
                    strcat(str, temp);

                    if (c == 'l')
                    {
                        c = b_fgetc(fp);

                        temp[0] = c;
                        strcat(str, temp);

                        if (!isalnum(c))
                        {
                            b_ungetc(c);
                            return "<loop>";
                        }
                    }
                }
            }
        }
        ungets(str, fp);
        c = b_fgetc(fp);
    }

    if (c == 'd') // def
    {
        temp[0] = c;
        strcat(str, temp);
        c = b_fgetc(fp);
        temp[0] = c;
        strcat(str, temp);


        if (c == 'e')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);

            if (c == 'f')
            {
                c = b_fgetc(fp);

                temp[0] = c;
                strcat(str, temp);

                if (!isalnum(c))
                {
                    b_ungetc(c);
                    return "<def>";
                }
            }
        }
        ungets(str, fp);
        c = b_fgetc(fp);
    }

    if (c == 'v') // var
    {
        temp[0] = c;
        strcat(str, temp);
        c = b_fgetc(fp);
        temp[0] = c;
        strcat(str, temp);


        if (c == 'a')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);

            if (c == 'r')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);

                if (!isalnum(c))
                {
                    b_ungetc(c);
                    return "<var>";
                }
            }
        }
        ungets(str, fp);
        c = b_fgetc(fp);
    }

    if (c == 's') // str
    {
        temp[0] = c;
        strcat(str, temp);
        c = b_fgetc(fp);
        temp[0] = c;
        strcat(str, temp);


        if (c == 't')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);


            if (c == 'r')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);

                if (!isalnum(c))
                {
                    b_ungetc(c);
                    return "<str>";
                }
            }
        }
        ungets(str, fp);
        c = b_fgetc(fp);
    }

    if (c == 'b') // bool
    {
        temp[0] = c;
        strcat(str, temp);
        c = b_fgetc(fp);
        temp[0] = c;
        strcat(str, temp);


        if (c == 'o')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);


            if (c == 'o')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);


                if (c == 'l')
                {
                    c = b_fgetc(fp);

                    temp[0] = c;
                    strcat(str, temp);

                    if (!isalnum(c))
                    {
                        b_ungetc(c);
                        return "<bool>";
                    }
                }
            }
        }
        ungets(str, fp);
        c = b_fgetc(fp);
    }

    if (c == 'r') // return
    {
        temp[0] = c;
        strcat(str, temp);
        c = b_fgetc(fp);
        temp[0] = c;
        strcat(str, temp);

        if (c == 'e')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);


            if (c == 't')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);


                if (c == 'u')
                {
                    c = b_fgetc(fp);
                    temp[0] = c;
                    strcat(str, temp);


                    if (c == 'r')
                    {
                        c = b_fgetc(fp);
                        temp[0] = c;
                        strcat(str, temp);


                        if (c == 'n')
                        {
                            c = b_fgetc(fp);
                            temp[0] = c;
                            strcat(str, temp);

                            if (!isalnum(c))
                            {
                                b_ungetc(c);
                                return "<return>";
                            }
                        }

                    }
                }
            }
        }
        ungets(str, fp);
        c = b_fgetc(fp);
    }


    if (c == 'm') // main
    {
        temp[0] = c;
        strcat(str, temp);
        c = b_fgetc(fp);
        temp[0] = c;
        strcat(str, temp);


        if (c == 'a')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);


            if (c == 'i')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);


                if (c == 'n')
                {
                    c = b_fgetc(fp);
                    temp[0] = c;
                    strcat(str, temp);

                    if (!isalnum(c))
                    {
                        b_ungetc(c);
                        return "<main>";
                    }
                }
            }
        }
        ungets(str, fp);
        c = b_fgetc(fp);
    }

    if (c == 'o') // or
    {
        temp[0] = c;
        strcat(str, temp);

        c = b_fgetc(fp);
        temp[0] = c;
        strcat(str, temp);


        if (c == 'r')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);

            if (!isalnum(c))
            {
                b_ungetc(c);
                return "<||>";
            }
        }
        ungets(str, fp);
        c = b_fgetc(fp);
    }

    if (c == 'a') // and
    {
        temp[0] = c;
        strcat(str, temp);
        c = b_fgetc(fp);
        temp[0] = c;
        strcat(str, temp);


        if (c == 'n')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);


            if (c == 'd')
            {
                c = b_fgetc(fp);

                temp[0] = c;
                strcat(str, temp);

                if (!isalnum(c))
                {
                    b_ungetc(c);
                    return "<&&>";
                }
            }
        }
        ungets(str, fp);
        c = b_fgetc(fp);
    }


    if (c == 'e') // equal
    {
        temp[0] = c;
        strcat(str, temp);
        c = b_fgetc(fp);
        temp[0] = c;
        strcat(str, temp);


        if (c == 'q')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);


            if (c == 'u')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);


                if (c == 'a')
                {
                    c = b_fgetc(fp);
                    temp[0] = c;
                    strcat(str, temp);


                    if (c == 'l')
                    {
                        c = b_fgetc(fp);

                        temp[0] = c;
                        strcat(str, temp);

                        if (!isalnum(c))
                        {
                            b_ungetc(c);
                            return "<==>";
                        }
                    }
                }
            }
        }
        ungets(str, fp);
        c = b_fgetc(fp);
    }

    if (c == 'n') // not_equal
    {
        temp[0] = c;
        strcat(str, temp);
        c = b_fgetc(fp);
        temp[0] = c;
        strcat(str, temp);


        if (c == 'o')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);


            if (c == 't')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);

                if (c == '_')
                {
                    c = b_fgetc(fp);
                    temp[0] = c;
                    strcat(str, temp);

                    if (c == 'e')
                    {
                        c = b_fgetc(fp);
                        temp[0] = c;
                        strcat(str, temp);


                        if (c == 'q')
                        {
                            c = b_fgetc(fp);
                            temp[0] = c;
                            strcat(str, temp);


                            if (c == 'u')
                            {
                                c = b_fgetc(fp);
                                temp[0] = c;
                                strcat(str, temp);


                                if (c == 'a')
                                {
                                    c = b_fgetc(fp);
                                    temp[0] = c;
                                    strcat(str, temp);


                                    if (c == 'l')
                                    {
                                        c = b_fgetc(fp);

                                        temp[0] = c;
                                        strcat(str, temp);

                                        if (!isalnum(c))
                                        {
                                            b_ungetc(c);
                                            return "<~=>";
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        ungets(str, fp);
        c = b_fgetc(fp);
    }

    if (c == 'b') // bigger_than
    {
        temp[0] = c;
        strcat(str, temp);
        c = b_fgetc(fp);
        temp[0] = c;
        strcat(str, temp);

        if (c == 'i')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);


            if (c == 'g')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);


                if (c == 'g')
                {
                    c = b_fgetc(fp);
                    temp[0] = c;
                    strcat(str, temp);


                    if (c == 'e')
                    {
                        c = b_fgetc(fp);
                        temp[0] = c;
                        strcat(str, temp);

                        if (c == 'r')
                        {
                            c = b_fgetc(fp);
                            temp[0] = c;
                            strcat(str, temp);


                            if (c == '_')
                            {
                                c = b_fgetc(fp);
                                temp[0] = c;
                                strcat(str, temp);


                                if (c == 't')
                                {
                                    c = b_fgetc(fp);
                                    temp[0] = c;
                                    strcat(str, temp);


                                    if (c == 'h')
                                    {
                                        c = b_fgetc(fp);
                                        temp[0] = c;
                                        strcat(str, temp);


                                        if (c == 'a')
                                        {
                                            c = b_fgetc(fp);
                                            temp[0] = c;
                                            strcat(str, temp);


                                            if (c == 'n')
                                            {
                                                c = b_fgetc(fp);
                                                temp[0] = c;
                                                strcat(str, temp);

                                                if (!isalnum(c))
                                                {
                                                    b_ungetc(c);
                                                    return "<>>";
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        ungets(str, fp);
        c = b_fgetc(fp);
    }

    if (c == 's') // smaller_than
    {
        temp[0] = c;
        strcat(str, temp);
        c = b_fgetc(fp);
        temp[0] = c;
        strcat(str, temp);


        if (c == 'm')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);


            if (c == 'a')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);


                if (c == 'l')
                {
                    c = b_fgetc(fp);
                    temp[0] = c;
                    strcat(str, temp);


                    if (c == 'l')
                    {
                        c = b_fgetc(fp);
                        temp[0] = c;
                        strcat(str, temp);


                        if (c == 'e')
                        {
                            c = b_fgetc(fp);
                            temp[0] = c;
                            strcat(str, temp);


                            if (c == 'r')
                            {
                                c = b_fgetc(fp);
                                temp[0] = c;
                                strcat(str, temp);


                                if (c == '_')
                                {
                                    c = b_fgetc(fp);
                                    temp[0] = c;
                                    strcat(str, temp);


                                    if (c == 't')
                                    {
                                        c = b_fgetc(fp);
                                        temp[0] = c;
                                        strcat(str, temp);


                                        if (c == 'h')
                                        {
                                            c = b_fgetc(fp);
                                            temp[0] = c;
                                            strcat(str, temp);


                                            if (c == 'a')
                                            {
                                                c = b_fgetc(fp);
                                                temp[0] = c;
                                                strcat(str, temp);

                                                if (c == 'n')
                                                {
                                                    c = b_fgetc(fp);

                                                    temp[0] = c;
                                                    strcat(str, temp);

                                                    if (!isalnum(c))
                                                    {
                                                        b_ungetc(c);
                                                        return "<<>";
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        ungets(str, fp);
        c = b_fgetc(fp);
    }

    if (c == 'T') // True
    {
        temp[0] = c;
        strcat(str, temp);
        c = b_fgetc(fp);
        temp[0] = c;
        strcat(str, temp);


        if (c == 'r')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);


            if (c == 'u')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);


                if (c == 'e')
                {
                    c = b_fgetc(fp);
                    temp[0] = c;
                    strcat(str, temp);
                    if (!isalnum(c))
                    {
                        b_ungetc(c);
                        return "<boolean_literal>";
                    }
                }
            }
        }
        ungets(str, fp);
        c = b_fgetc(fp);
    }

    if (c == 'F') // False
    {
        temp[0] = c;
        strcat(str, temp);
        c = b_fgetc(fp);
        temp[0] = c;
        strcat(str, temp);


        if (c == 'a')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);


            if (c == 'l')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);


                if (c == 's')
                {
                    c = b_fgetc(fp);
                    temp[0] = c;
                    strcat(str, temp);

                    if (c == 'e')
                    {
                        c = b_fgetc(fp);

                        temp[0] = c;
                        strcat(str, temp);

                        if (!isalnum(c))
                        {
                            b_ungetc(c);
                            return "<boolean_literal>";
                        }
                    }
                }
            }
        }
        ungets(str, fp);
        c = b_fgetc(fp);
    }

    if (c == 'p') // Printiza
    {
        temp[0] = c;
        strcat(str, temp);

        c = b_fgetc(fp);
        temp[0] = c;
        strcat(str, temp);

        if (c == 'r')
        {
            c = b_fgetc(fp);

            temp[0] = c;
            strcat(str, temp);
            if (c == 'i')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);

                if (c == 'n')
                {
                    c = b_fgetc(fp);
                    temp[0] = c;
                    strcat(str, temp);

                    if (c == 't')
                    {
                        c = b_fgetc(fp);
                        temp[0] = c;
                        strcat(str, temp);

                        if (c == 'i')
                        {
                            c = b_fgetc(fp);
                            temp[0] = c;
                            strcat(str, temp);

                            if (c == 'z')
                            {
                                c = b_fgetc(fp);
                                temp[0] = c;
                                strcat(str, temp);

                                if (c == 'a')
                                {
                                    c = b_fgetc(fp);
                                    temp[0] = c;
                                    strcat(str, temp);

                                    if (!isalnum(c))
                                    {
                                        b_ungetc(c);
                                        return "<printiza>";
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        ungets(str, fp);
        c = b_fgetc(fp);
    }

    if (c == 'i') // inputiza
    {
        temp[0] = c;
        strcat(str, temp);
        c = b_fgetc(fp);
        temp[0] = c;
        strcat(str, temp);

        if (c == 'n')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);

            if (c == 'p')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);

                if (c == 'u')
                {
                    c = b_fgetc(fp);
                    temp[0] = c;
                    strcat(str, temp);

                    if (c == 't')
                    {
                        c = b_fgetc(fp);
                        temp[0] = c;
                        strcat(str, temp);

                        if (c == 'i')
                        {
                            c = b_fgetc(fp);
                            temp[0] = c;
                            strcat(str, temp);

                            if (c == 'z')
                            {
                                c = b_fgetc(fp);
                                temp[0] = c;
                                strcat(str, temp);

                                if (c == 'a')
                                {
                                    c = b_fgetc(fp);

                                    temp[0] = c;
                                    strcat(str, temp);

                                    if (!isalnum(c))
                                    {
                                        b_ungetc(c);
                                        return "<inputiza>";
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        ungets(str, fp);
        c = b_fgetc(fp);
    }

    if (isalpha(c))
    {
        c = b_fgetc(fp);
        while (isalnum(c))
        {
            c = b_fgetc(fp);
        }
        b_ungetc(c);
        return "<function>";
    }

    if ( c == '!' )
    {
        c = b_fgetc(fp);
        temp[0] = c;
        strcat(str, temp);

        if (isalpha(c))
        {
            c = b_fgetc(fp);
            while (isalnum(c))
            {
                c = b_fgetc(fp);
            }
            b_ungetc(c);
            return "<variable>";
        }
        b_ungetc(c);
        return "<ERROR>";
    }

    if (c == '"' )
    {
        c = b_fgetc(fp);
        while ((c != '"') && (c != EOF) && (c != 10))
        {
            c = b_fgetc(fp);
        }
        if(c == '"' )
        {
            return "<string_literal>";
        }
        else
        {
            return "<ERROR>";
        }
    }

    if (c == '-')
    {
        temp[0] = c;
        strcat(str, temp);
        c = b_fgetc(fp);
        temp[0] = c;
        strcat(str, temp);

        if (c == '>')
        {
            return "<->>";
        }

        ungets(str, fp);
        c = b_fgetc(fp);
    }

    if ((isdigit(c)) || (c == '-')) // integer and float literals
    {
        temp[0] = c;
        strcat(str, temp);
        if (c == '-')
        {
            c = b_fgetc(fp);
            temp[0] = c;
            strcat(str, temp);
        }
        if (isdigit(c))
        {
            while (isdigit(c))
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);
            }

            if (c == '.')
            {
                c = b_fgetc(fp);
                temp[0] = c;
                strcat(str, temp);
                if (isdigit(c))
                {
                    while (isdigit(c))
                    {
                        c = b_fgetc(fp);
                        temp[0] = c;
                        strcat(str, temp);
                    }

                    b_ungetc(c);
                    return "<float_literal>";
                }
                else
                {
                    b_ungetc(c);
                    return "<ERROR>";
                }
            }
            else
            {
                b_ungetc(c);
                return "<integer_literal>";
            }

        }
        ungets(str, fp);
        c = b_fgetc(fp);
    }

    if ( c == '(' )
    {
        return "<(>";
    }

    else if ( c == ')' )
    {
        return "<)>" ;
    }

    else if ( c == ',' )
    {
        return "<,>";
    }

    else if ( c == '.' )
    {
        return "<.>";
    }

    else if ( c == ':' )
    {
        return "<:>";
    }

    else if ( c == ';' )
    {
        return "<;>";
    }

    else if (c == '{' )
    {
        return "<{>";
    }

    else if ( c == '}' )
    {
        return "<}>";
    }

    else if ( c == '[' )
    {
        return "<[>";
    }

    else if ( c == ']' )
    {
        return "<]>";
    }

    // equal sign
    else if ( c == '=' )
    {
        c = b_fgetc(fp);
        if (c == '=' )
        {
            return "<==>";
        }
        else if (c == '>')
        {
            return "<>=>";
        }
        else if (c == '<')
        {
            return "<<=>";
        }
        else
        {
            b_ungetc(c);
            return "<=>";
        }
    }

    // Plus
    else if ( c == '+' )
    {
        return "<+>";
    }

    // Minus
    else if ( c == '-' )
    {
        return "<->";
    }

    // Multiply
    else if ( c == '*' )
    {
        return "<*>";
    }

    // Power
    else if ( c == '^' )
    {
        return "<^>";
    }

    else if ( c == '%' )
    {
        return "<^>";
    }
    // Divides
    else if ( c == '/' )
    {
        c = b_fgetc(fp);
        if ( c == '/' )
        {
            return "<//>";
        }
        else
        {
            b_ungetc(c);
            return "</>";
        }
    }


    else if ( c == '>' )
    {
        c = b_fgetc(fp);
        if ( c == '=')
        {
            return "<>=>";
        }
        else
        {
            b_ungetc(c);
            return "<>>";
        }
    }

    else if ( c == '<')
    {
        c = fgetc (fp);
        if ( c == '=' )
        {
            return "<<=>";
        }
        else
        {
            b_ungetc(c);
            return "<<>";
        }
    }

    else if ( c == '~' )
    {
        c = b_fgetc(fp);
        if ( c == '=' )
        {
            return "<~=>";
        }
        else
        {
            b_ungetc(c);
            return "<~>";
        }
    }

    else if ( c == '|' )
    {
        c = b_fgetc(fp);
        if ( c == '|' )
        {
            return "<||>";
        }
        else
        {
            b_ungetc(c);
            return "<|>";
        }
    }

    else if ( c == '&' )
    {
        c = b_fgetc(fp);
        if ( c == '&' )
        {
            return "<&&>";
        }
        else
        {
            b_ungetc(c);
            return "<&>";
        }
    }

    if (c == EOF)
    {
        return "EOF";
    }

    while ((c != EOF) && (c != ' ') && (c != 10))
    {
        c = b_fgetc(fp);
    }
    return "<ERROR>";
}

void main()
{
    FILE *sourcef, *tokensf;
    sourcef = fopen("source.txt", "r");
    if (sourcef == NULL)
    {
        printf("Source file could not be opened");
    }
    else
    {
        tokensf = fopen("tokens.txt", "w");
        if (tokensf == NULL)
        {
            printf("Token file could not be opened");
        }
        else
        {
            char* token = scan_token(sourcef);
            printf("%s", token);
            while (strcmp(token, "EOF"))
            {
                if (strcmp(token, "\n") == 0)
                {
                    fprintf(tokensf, "%s", token);
                    fflush(tokensf);
                }
                else
                {
                    fprintf(tokensf, "%s ", token);
                    fflush(tokensf);
                }

                token = scan_token(sourcef);
                printf("%s", token);
            }
        }

        fclose(sourcef);
        fclose(tokensf);
    }
}
