open ReasonUrql;

let client = Client.make(~url="https://formidadog-ql.now.sh", ());

ReactDOMRe.renderToElementWithId(
  <Provider value=client> <Example client /> </Provider>,
  "root",
);