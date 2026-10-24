import { defineConfig } from 'vitepress';
const api = 'https://codeflare.lucius7.dev/';

export default defineConfig({
  lang: 'zh-CN',
  title: 'CodeFlare Docs',
  description: 'CodeFlare 题库 API 参考、数据契约与项目规范。',
  base: '/docs/',
  srcDir: '.content',
  cleanUrls: false,
  head: [['link', { rel: 'icon', href: '/favicon.svg' }]],
  sitemap: { hostname: `${api}docs/` },
  themeConfig: {
    siteTitle: 'CodeFlare / Docs',
    nav: [
      { text: 'API 参考', link: '/api/' },
      { text: '项目规范', link: '/conventions' },
      { text: '题库 ↗', link: api },
    ],
    sidebar: [
      { text: '文档', items: [{ text: '概览', link: '/' }, { text: '文档导航', link: '/handbook' }] },
      { text: 'API REFERENCE', items: [
        { text: '接入与契约', link: '/api/' },
        { text: 'GET 数据快照', link: '/api/snapshot' },
        { text: 'GET 最近提交', link: '/api/updates' },
        { text: '数据模型', link: '/api/models' },
        { text: '源码与外部接口', link: '/api/dependencies' },
        { text: '错误与兼容规范', link: '/api/standards' },
      ] },
      { text: '项目规范', collapsed: false, items: [
        { text: '仓库 README', link: '/repository' },
        { text: '项目结构', link: '/structure' },
        { text: '源码目录清单', link: '/catalog' },
        { text: '编译与验证', link: '/quick-start' },
        { text: '代码与归档', link: '/conventions' },
        { text: '贡献流程', link: '/contributing' },
      ] },
      { text: '维护', collapsed: false, items: [
        { text: '开发工具', link: '/tooling' },
        { text: '文档部署', link: '/maintenance' },
        { text: '题库 Pages', link: '/pages' },
      ] },
    ],
    search: { provider: 'local', options: { locales: { root: { translations: {
      button: { buttonText: '搜索文档', buttonAriaLabel: '搜索文档' },
      modal: { noResultsText: '没有找到相关内容', resetButtonTitle: '清除搜索',
        footer: { selectText: '选择', navigateText: '切换', closeText: '关闭' } },
    } } } } },
    outline: { level: [2, 3], label: '本页目录' },
    docFooter: { prev: '上一页', next: '下一页' },
    sidebarMenuLabel: '目录',
    returnToTopLabel: '回到顶部',
    darkModeSwitchLabel: '切换深色模式',
    socialLinks: [{ icon: 'github', link: 'https://github.com/xw7qwq/solutions/tree/docs/project-guide' }],
    editLink: { text: '编辑本页', pattern: ({ relativePath }) => {
      const source = relativePath === 'repository.md' ? 'README.md'
        : relativePath === 'contributing.md' ? 'CONTRIBUTING.md'
        : relativePath === 'handbook.md' ? 'docs/README.md' : `docs/${relativePath}`;
      return `https://github.com/xw7qwq/solutions/edit/docs/project-guide/${source}`;
    } },
  },
});
