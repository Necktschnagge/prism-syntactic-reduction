git_username=${1}
git_access_token=${2}

git_hash_last_commit=$(git rev-parse HEAD)
git_branch_for_results="ci-generate-models-${git_hash_last_commit}"
git_repo_url=$(git remote get-url origin)
user_repo_id=$(echo "${git_repo_url}" | sed -E 's/https:\/\/\w*.\w*\///' | sed -E 's/\.git//')

git switch -c ${git_branch_for_results} #switch to new branch pointing to current HEAD
cp script/ci/azure-yml/distribute-generated-models.yml azure-pipelines.yml
git add *
git status
git -c user.name="CI for Necktschnagge" -c user.email="ci-for-necktschnagge@example.org" commit -m "Automatic upload of all generated models"
git status
git push https://${git_username}:${git_access_token}@github.com/${user_repo_id} ${git_branch_for_results}