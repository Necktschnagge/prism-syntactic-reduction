remote_name=${1}
echo ${remote_name}

if [[ -z "${remote_name}" ]]; then
	remote_name=origin
fi
echo ${remote_name}
git fetch
#git branch -r | grep "${remote_name}/ci-generate-models-"
git branch -r | grep ${remote_name}/ci-generate-models- | while read line 
do
	echo Delte branch: ${line}
	branch_name=$(echo ${line} | sed -E "s/${remote_name}\///")
	echo pure branch name: ${branch_name}
	git push ${remote_name} --delete ${branch_name}
done
