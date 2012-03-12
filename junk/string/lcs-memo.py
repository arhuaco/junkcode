#!/usr/bin/python

def LCS(S, T, memo):

    if not len(S) or not len(T):
        return ''

    if memo.has_key((S,T)):
        return memo[(S,T)]

    if S[0] == T[0]:
        ret = S[0] + LCS(S[1:],  T[1:], memo)
    else:
        s1 = LCS(S[1:], T, memo)
        s2 = LCS(S, T[1:], memo)
        ret = s1 if len(s1) > len(s2) else s2

    memo[(S,T)] = ret
    #Uncomment if you want to see how the cache is created.
    #print (S,T), '=', ret

    return ret

print LCS('abcdefghijklmnzyxwvutsrqpo', 'opqrstuvwxyzabcdefghijklmn', {})
