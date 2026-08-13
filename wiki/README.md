# wiki/ — GitHub Wiki исходники

> Эта папка содержит **исходники** для GitHub Wiki репозитория.

## Как залить в GitHub Wiki

У GitHub Wiki есть **отдельный git-репозиторий** (обычно `https://github.com/<user>/<repo>.wiki.git`).
Чтобы синхронизировать эту папку с Wiki:

```powershell
# 1. Клонировать wiki-репо
git clone https://github.com/kochatattoo/NovaEngine.wiki.git wiki-tmp

# 2. Скопировать файлы
Copy-Item -Recurse -Force wiki\* wiki-tmp\

# 3. Закоммитить и запушить
cd wiki-tmp
git add -A
git commit -m "Sync wiki from main repo"
git push origin master
cd ..
Remove-Item -Recurse -Force wiki-tmp
```

Или в одну команду через CI (GitHub Actions):
```yaml
name: Sync Wiki
on:
  push:
    paths: ['wiki/**']
jobs:
  sync:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Push to wiki
        run: |
          git clone https://github.com/${{ github.repository }}.wiki.git wiki-out
          cp -r wiki/* wiki-out/
          cd wiki-out
          git config user.email "bot@example.com"
          git config user.name "Wiki Bot"
          git add -A
          git diff --staged --quiet || git commit -m "Sync from main"
          git push
```

## Файлы

| Файл | Назначение |
|---|---|
| `Home.md` | Главная страница (оглавление) |
| `_Sidebar.md` | Боковая панель (отображается на каждой странице) |
| `Getting-Started.md` | Quick start |
| `Architecture.md` | Обзор архитектуры |
| `Subsystem-*.md` | По странице на подсистему (Core, Renderer, Scene, ECS, Input, UI, Lua) |
| `Scripting-API.md` | Lua API |
| `Match3-Game.md` | Демо-игра |
| `Building-from-source.md` | Детали сборки |
| `Roadmap.md` | План развития |
| `Coding-Conventions.md` | Правила кода для контрибьюторов |

> ⚠️ **TODO:** все страницы пока **заготовки**. Заполни содержимое на основе [`docs/`](../docs/README.md) — там подробная документация.

## Соглашения

- Один файл = одна страница.
- Имя файла = URL Wiki (без `.md`).
- Без подпапок (GitHub Wiki плохо их поддерживает).
- Начинай с `# Заголовок` (это будет имя страницы в Wiki).
