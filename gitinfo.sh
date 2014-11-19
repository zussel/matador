#!/bin/bash

ID=`git --no-pager log -1 --pretty=format:%H`
AUTHOR_NAME=`git --no-pager log -1 --pretty=format:%aN`
AUTHOR_EMAIL=`git --no-pager log -1 --pretty=format:%ae`
COMMITTER_NAME=`git --no-pager log -1 --pretty=format:%cN`
COMMITTER_EMAIL=`git --no-pager log -1 --pretty=format:%ce`
MESSAGE=`git --no-pager log -1 --pretty=format:%s`

BRANCH=`git rev-parse --abbrev-ref HEAD`

echo "'head': {"
echo "    'id': '$ID',"
echo "    'author_name': '$AUTHOR_NAME',"
echo "    'author_email': '$AUTHOR_EMAIL',"
echo "    'committer_name': '$COMMITTER_NAME',"
echo "    'committer_email': '$COMMITTER_EMAIL',"
echo "    'message': '$MESSAGE'"
echo "}, {"
echo "'branch': '$BRANCH',"
echo "'remotes': "
