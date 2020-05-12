const path = require("path");

module.exports = {
  entry: path.join(__dirname, "src/index.bs.js"),
  mode: "development",
  output: {
    path: path.resolve(__dirname, "build"),
    publicPath: "/public/",
    filename: "index.js",
  },
  devServer: {
    contentBase: path.resolve(__dirname, "public"),
    port: 3000,
  },
};
