import re

# Validate an email.
# This is a simple regex.
# I found it it in google groups and I liked it.
# It's simple. If you know a better way to do this please email
# nelsoneci@gmail.com.

def valid_email(email):
  x =  re.match("^[0-9a-z]([-+_.]?[0-9a-z])*@[0-9a-z]([-.]?[0-9a-z])*\\.[a-z]{2,4}$", email);
  return x != None; # True if the email is valid.

def valid_name(name):
  x = re.math("", name)
