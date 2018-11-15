# Getting started

Install ruby and ruby docs (Arch Linux example)

```bash
sudo pacman -S ruby ruby-docs
```

Update gems and install jekyll

```bash
gem update
gem install jekyll
```

Add to .bashrc:

```bash
# ruby stuff
PATH="$(ruby -e 'puts Gem.user_dir')/bin:$PATH"
export GEM_HOME=$(ruby -e 'puts Gem.user_dir')
```

Install bundler for Gemfile

```bash
gem install bundler
```

Install packages via bundler

```bash
bundle install
```

Serve page

```bash
bundle exec jekyll serve
```
