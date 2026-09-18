const fs = require('node:fs');
const { version } = require('../package.json');
if (process.env.GITHUB_REF_NAME !== `v${version}`) {
  throw new Error(`Release tag must be v${version}.`);
}
fs.appendFileSync(
  process.env.GITHUB_ENV,
  `NPM_DIST_TAG=${version.includes('-') ? 'next' : 'latest'}\n`,
);
