it_username=${1}
git_access_token=${2}
branch_name=$(echo ${3} | sed -E 's/refs\/heads\///')



parent_branch_name=$(echo ${branch_name} | sed -E 's/\+\+\+\d*//')



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
id=$(cat "./RESULTS/prism_model/id.txt")
cp ./RESULTS/prism_model/prism_data.json ./RESULTS/${id}/prism_data.json
git fetch https://${git_username}:${git_access_token}@github.com/${user_repo_id}
git checkout ${parent_branch_name}
rm ./azure-pipelines.yml
git add -u ./azure-pipelines.yml
git add ./RESULTS/${id}/prism_data.json
git status
git -c user.name="CI for Necktschnagge" -c user.email="ci-for-necktschnagge@example.org" commit -m "upload extracted data of sub branch ${id}"
pushed_successfully=false
while [[ !(${pushed_successfully}) ]]
do
	git fetch https://${git_username}:${git_access_token}@github.com/${user_repo_id}
	git merge origin/${parent_branch_name}
	git push https://${git_username}:${git_access_token}@github.com/${user_repo_id} ${parent_branch_name} && pushed_successfully=true
done

count_results=0
count_subdirs=0

cd ./RESULTS
for D in */ ; do
	echo iterator: ${D}
	number=$(echo ${D} | sed -E 's/\///')
	echo number: ${number}
	file_name=${id}/prism_data.json
	if [[ -f "${file_name}" ]]; then
		count_results=$((count_results+1))
	fi
	count_subdirs=$((count_subdirs+1))	
done
cd ..

if [[ count_results -eq count_subdirs ]]; then
	echo "All results are collected in parent branch. Adding the next steps..."
	cp script/ci/azure-yml/combine-data-of-all-reduced-models.yml azure-pipelines.yml
	git add azure-pipelines.yml
	git status
	git -c user.name="CI for Necktschnagge" -c user.email="ci-for-necktschnagge@example.org" commit -m "upload azure-pipelines.yml for combining all results"
	git push https://${git_username}:${git_access_token}@github.com/${user_repo_id} ${parent_branch_name}
fi


