git_username=${1}
git_access_token=${2}
branch_name=$(echo ${3} | sed -E 's/refs\/heads\///')



parent_branch_name=$(echo ${branch_name} | perl -pe 's/\+\+\+\d*//g')



git_hash_last_commit=$(git rev-parse HEAD)
git_branch_for_results="ci-generate-models-${git_hash_last_commit}"
git_repo_url=$(git remote get-url origin)
user_repo_id=$(echo "${git_repo_url}" | sed -E 's/https:\/\/\w*.\w*\///' | sed -E 's/\.git//')
 

#git switch -c ${git_branch_for_results} #switch to new branch pointing to current HEAD
#cp script/ci/azure-yml/distribute-generated-models.yml azure-pipelines.yml
#git add *
#git status
#git -c user.name="CI for Necktschnagge" -c user.email="ci-for-necktschnagge@example.org" commit -m "Automatic upload of generated models"
#git status
#git push https://${git_username}:${git_access_token}@github.com/${user_repo_id} ${git_branch_for_results}


#./RESULTS/prism_model/id.txt
git remote
echo list all branches on remote...
git branch -r

id=$(cat "./RESULTS/prism_model/id.txt")
cp ./RESULTS/prism_model/prism_data.json ./RESULTS/${id}/prism_data.json
rm -r ./RESULTS/prism_model/
git fetch https://${git_username}:${git_access_token}@github.com/${user_repo_id}
git switch -c ${parent_branch_name} origin/${parent_branch_name}
git status
rm ./azure-pipelines.yml
git add -u ./azure-pipelines.yml
git add ./RESULTS/${id}/prism_data.json
git status
git -c user.name="CI for Necktschnagge" -c user.email="ci-for-necktschnagge@example.org" commit -m "upload extracted data of sub branch ${id}"

pushed_successfully=0
while [[ ${pushed_successfully} -eq 0 ]]; do
	echo fetch ...
	git fetch origin
	echo merge ...
	git -c user.name="CI for Necktschnagge" -c user.email="ci-for-necktschnagge@example.org" merge origin/${parent_branch_name}
	echo rebase ...
	git -c user.name="CI for Necktschnagge" -c user.email="ci-for-necktschnagge@example.org" rebase origin/${parent_branch_name}
	echo push ...
	###use rebase instead!!!!
	git push https://${git_username}:${git_access_token}@github.com/${user_repo_id} ${parent_branch_name} && pushed_successfully=1
done

count_results=0
count_subdirs=0

cd ./RESULTS
for D in */ ; do
	echo iterator: ${D}
	number=$(echo ${D} | sed -E 's/\///')
	echo number: ${number}
	file_name=./${number}/prism_data.json
	echo Check for file: ${file_name}
	ls -la ${D}
	if [[ -f ${file_name} ]]; then
		count_results=$((count_results+1))
	fi
	count_subdirs=$((count_subdirs+1))
	echo count_results: ${count_results}
	echo count_subdirs: ${count_subdirs}
done
cd ..

if [[ count_results -eq count_subdirs ]]; then
	echo "All results are collected in parent branch. Adding the next steps..."
	cp script/ci/azure-yml/combine-data-of-all-reduced-models.yml azure-pipelines.yml
	git add azure-pipelines.yml
	git status
	git -c user.name="CI for Necktschnagge" -c user.email="ci-for-necktschnagge@example.org" commit -m "upload azure-pipelines.yml for combining all results"
	git push https://${git_username}:${git_access_token}@github.com/${user_repo_id} ${parent_branch_name}
	#./script/ci/delete-ci-branches.sh # there might be parallel runs, this may cause interruptions on them
fi


