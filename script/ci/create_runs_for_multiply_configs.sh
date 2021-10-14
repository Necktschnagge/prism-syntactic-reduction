git_username=${1}
git_access_token=${2}
branch_name=$(echo ${3} | sed -E 's/refs\/heads\///')

git_hash_last_commit=$(git rev-parse HEAD)
branch_name_ci="ci-distributed-json-${git_hash_last_commit}"
git_repo_url=$(git remote get-url origin)
user_repo_id=$(echo "${git_repo_url}" | sed -E 's/https:\/\/\w*.\w*\///' | sed -E 's/\.git//')
 

#git switch -c ${git_branch_for_results} #switch to new branch pointing to current HEAD
#cp script/ci/azure-yml/distribute-generated-models.yml azure-pipelines.yml
#git add *
#git status
#git -c user.name="CI for Necktschnagge" -c user.email="ci-for-necktschnagge@example.org" commit -m "Automatic upload of generated models"
#git status
#git push https://${git_username}:${git_access_token}@github.com/${user_repo_id} ${git_branch_for_results}

#cd ./res/
id=0
for filename in ./res/all_configs/*.json; do
	echo considering file: ${filename}
	cp ${filename} ./res/config.json
	cp ./script/ci/azure-yml/create-all-models-for-one-specific-config-json.yml azure-pipelines.yml

	sub_branch_name=${branch_name_ci}+++${filename}
	git switch -c ${sub_branch_name} #switch to new branch pointing to current HEAD
	git add ./res/config.json
	git add ./azure-pipelines.yml
	echo added git files.
	git status
	git -c user.name="CI for Necktschnagge" -c user.email="ci-for-necktschnagge@example.org" commit -m "consider one specific json: (${filename})"
	git status
	git branch
	git push https://${git_username}:${git_access_token}@github.com/${user_repo_id} ${sub_branch_name}
	
	git reset --hard HEAD~1
	
	id=$((${id}+1))
done

