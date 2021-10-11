remote_name=${1}

if [[ ${remote_name} -eq ""]]; then
	remote_name=origin
fi

git branch -r | grep ${remote_name}/ci-generate-models- | while read line 
do
	echo Delte branch: ${line}
	branch_name=$(echo ${line} | sed -E 's/${remote_name}\///')
	echo pure branch name: ${branch_name}
	#git push ${remote_name} --delete ${branch_name}
done
