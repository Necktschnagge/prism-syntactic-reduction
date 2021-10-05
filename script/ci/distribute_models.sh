git_username=${1}
git_access_token=${2}
branch_name=$(echo ${3} | sed -E 's/refs\/heads\///')

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


cd RESULTS
for D in `find . -type d`
do
	number=$(echo ${D} | sed -E 's/.\///')
	git switch -c ${branch_name}-${number} #switch to new branch pointing to current HEAD
	cp script/ci/azure-yml/run-prism-on-one-model.yml azure-pipelines.yml
    cp D ../prism_model
	git add *
	git status
	git -c user.name="CI for Necktschnagge" -c user.email="ci-for-necktschnagge@example.org" commit -m "Automatic upload of generated models"
	git status
	git push https://${git_username}:${git_access_token}@github.com/${user_repo_id} ${git_branch_for_results}
	git reset --hard HEAD~1
	echo $D
	echo $ number
	
	
done

