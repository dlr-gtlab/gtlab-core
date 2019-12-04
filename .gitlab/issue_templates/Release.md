## Prepare release
- [ ] Create a release branch from the right revision of the ``master`` branch
      / release tag following the naming template `release/MAJOR.MINOR.PATCH`

### Preparation is finished when:
- [ ] [setup.py](setup.py) contains the right release number
- [ ] [CHANGELOG.md](CHANGELOG.md) is up-to-date
- [ ] Sphinx generated documentation user and developer documentation is up-to-date
- [ ] Reuse compliance check has been successfully checked (https://git.fsfe.org/reuse/tool)
- [ ] The relase build ran properly and indicated no problems
- [ ] The release package has been checked manually

## Create release
- [ ] Tag the release branch using the naming template ``MAJOR.MINOR.PATCH-stable``
- [ ] Merge the release into the ``master`` including the tags
- [ ] Add release notes to the GitLab tag (link to the changelog and attach release build artifacts)
- [ ] Delete the release branch

## Post release actions
- [ ] Set new development version
- [ ] Announce it


/label ~"type::release"
