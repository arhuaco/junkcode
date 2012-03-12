#!/usr/bin/python

def LCS(S, T):
    if not len(S) or not len(T):
        return ''

    if S[0] == T[0]:
        return S[0] + LCS(S[1:],  T[1:])

    s1 = LCS(S[1:], T)
    s2 = LCS(S, T[1:])

    if len(s1) > len(s2):
        return s1

    return s2

print LCS('a1b2c3d4e', 'zz1yy2xx3ww4vv')
