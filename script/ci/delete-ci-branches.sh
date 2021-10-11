git branch -r | grep origin/ci-generate-models- | while read line 
do
	echo Delte branch: ${line}
	branch_name=$(echo ${line} | sed -E 's/origin\///')
	echo pure branch name: ${branch_name}
	git push origin --delete ${branch_name}
done
